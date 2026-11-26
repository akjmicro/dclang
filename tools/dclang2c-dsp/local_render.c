#include <jack/jack.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void dclang2c_init(void);
void dclang2c_render(float *output, size_t frames);
int dclang2c_control_index(const char *name);

#if UINTPTR_MAX == UINT64_MAX
typedef double dclang_float;
#else
typedef float dclang_float;
#endif

dclang_float dclang2c_get_float(unsigned int index);
void dclang2c_set_float(unsigned int index, dclang_float value);

#ifndef DCLANG_LOCAL_SR
#define DCLANG_LOCAL_SR 44100
#endif

#ifndef DCLANG_OSC_PORT
#define DCLANG_OSC_PORT 7562
#endif

#define MAX_CONTROLS 64
#define CONTROL_TEXT 256

struct Control {
    char path[CONTROL_TEXT];
    char name[CONTROL_TEXT];
    float minimum;
    float maximum;
    int index;
    _Atomic uintptr_t bits;
};

static struct Control controls[MAX_CONTROLS];
static size_t control_count;
static unsigned int receiver_port = DCLANG_OSC_PORT;

static int receiver_socket = -1;
static pthread_t receiver_thread;
static _Atomic int receiver_running;

static jack_client_t *client;
static jack_port_t *left_port;
static jack_port_t *right_port;
static volatile sig_atomic_t running = 1;

static uintptr_t float_to_bits(dclang_float value)
{
    uintptr_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

static dclang_float bits_to_float(uintptr_t bits)
{
    dclang_float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static size_t osc_string_size(const unsigned char *packet, size_t bytes,
                              size_t offset)
{
    if (offset >= bytes)
        return 0;
    const void *end = memchr(packet + offset, 0, bytes - offset);
    if (!end)
        return 0;
    size_t length = (const unsigned char *)end - packet - offset;
    size_t padded = (length + 4) & ~(size_t)3;
    return padded <= bytes - offset ? padded : 0;
}

static size_t osc_put_string(unsigned char *packet, size_t capacity,
                             size_t offset, const char *value)
{
    size_t length = strlen(value);
    size_t padded = (length + 4) & ~(size_t)3;
    if (offset + padded > capacity)
        return 0;
    memcpy(packet + offset, value, length);
    memset(packet + offset + length, 0, padded - length);
    return offset + padded;
}

static size_t osc_put_float(unsigned char *packet, size_t capacity,
                            size_t offset, float value)
{
    if (offset + 4 > capacity)
        return 0;
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    bits = htonl(bits);
    memcpy(packet + offset, &bits, sizeof(bits));
    return offset + 4;
}

static struct Control *find_control(const char *path)
{
    for (size_t i = 0; i < control_count; ++i)
        if (controls[i].index >= 0 && strcmp(path, controls[i].path) == 0)
            return &controls[i];
    return NULL;
}

static void send_control_description(struct Control *control,
                                     struct sockaddr_in peer)
{
    unsigned char packet[256];
    size_t offset = osc_put_string(packet, sizeof(packet), 0, control->path);
    if (offset)
        offset = osc_put_string(packet, sizeof(packet), offset, ",fff");
    dclang_float current = bits_to_float(
        atomic_load_explicit(&control->bits, memory_order_acquire));
    if (offset)
        offset = osc_put_float(packet, sizeof(packet), offset, (float)current);
    if (offset)
        offset = osc_put_float(packet, sizeof(packet), offset, control->minimum);
    if (offset)
        offset = osc_put_float(packet, sizeof(packet), offset, control->maximum);
    if (!offset)
        return;
    peer.sin_port = htons(receiver_port + 1);
    sendto(receiver_socket, packet, offset, 0,
           (struct sockaddr *)&peer, sizeof(peer));
}

static void accept_packet(const unsigned char *packet, size_t bytes,
                          struct sockaddr_in peer)
{
    size_t path_size = osc_string_size(packet, bytes, 0);
    if (!path_size)
        return;
    size_t type_size = osc_string_size(packet, bytes, path_size);
    if (!type_size)
        return;
    struct Control *control = find_control((const char *)packet);
    if (!control)
        return;
    const char *types = (const char *)(packet + path_size);
    size_t argument = path_size + type_size;
    if (strcmp(types, ",f") == 0 && argument + 4 <= bytes) {
        uint32_t network_bits;
        float value;
        memcpy(&network_bits, packet + argument, sizeof(network_bits));
        network_bits = ntohl(network_bits);
        memcpy(&value, &network_bits, sizeof(value));
        if (value < control->minimum)
            value = control->minimum;
        if (value > control->maximum)
            value = control->maximum;
        atomic_store_explicit(&control->bits,
                              float_to_bits((dclang_float)value),
                              memory_order_release);
    } else if (strcmp(types, ",s") == 0) {
        size_t value_size = osc_string_size(packet, bytes, argument);
        if (value_size && strcmp((const char *)(packet + argument), "get") == 0)
            send_control_description(control, peer);
    }
}

static void *receive_osc(void *argument)
{
    (void)argument;
    unsigned char packet[256];
    while (atomic_load_explicit(&receiver_running, memory_order_acquire)) {
        struct sockaddr_in peer;
        socklen_t peer_size = sizeof(peer);
        ssize_t bytes = recvfrom(receiver_socket, packet, sizeof(packet), 0,
                                 (struct sockaddr *)&peer, &peer_size);
        if (bytes > 0)
            accept_packet(packet, (size_t)bytes, peer);
        else if (bytes < 0 && errno != EINTR &&
                 atomic_load_explicit(&receiver_running, memory_order_acquire))
            break;
    }
    return NULL;
}

static int start_receiver(void)
{
    receiver_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiver_socket < 0)
        return -1;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(receiver_port);
    if (bind(receiver_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(receiver_socket);
        receiver_socket = -1;
        return -1;
    }
    atomic_store_explicit(&receiver_running, 1, memory_order_release);
    if (pthread_create(&receiver_thread, NULL, receive_osc, NULL) != 0) {
        atomic_store_explicit(&receiver_running, 0, memory_order_release);
        close(receiver_socket);
        receiver_socket = -1;
        return -1;
    }
    return 0;
}

static void stop_receiver(void)
{
    if (receiver_socket < 0)
        return;
    atomic_store_explicit(&receiver_running, 0, memory_order_release);
    shutdown(receiver_socket, SHUT_RDWR);
    close(receiver_socket);
    receiver_socket = -1;
    pthread_join(receiver_thread, NULL);
}

static int load_controls(const char *path)
{
    if (!path)
        return 0;
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Unable to open OSC control config %s: %s\n",
                path, strerror(errno));
        return -1;
    }
    char line[1024];
    unsigned int line_number = 0;
    while (fgets(line, sizeof(line), file)) {
        ++line_number;
        char *cursor = line;
        while (*cursor == ' ' || *cursor == '\t')
            ++cursor;
        if (*cursor == '#' || *cursor == '\n' || *cursor == '\0')
            continue;
        if (control_count == MAX_CONTROLS) {
            fprintf(stderr, "%s: too many controls (maximum %u)\n",
                    path, MAX_CONTROLS);
            fclose(file);
            return -1;
        }
        struct Control *control = &controls[control_count];
        unsigned int port;
        double minimum;
        double maximum;
        char extra;
        int fields = sscanf(cursor, "%u %255s %255s %lf %lf %c", &port,
                            control->path, control->name, &minimum, &maximum,
                            &extra);
        if (fields != 5 || control->path[0] != '/' || minimum > maximum) {
            fprintf(stderr, "%s:%u: expected: port /path variable min max\n",
                    path, line_number);
            fclose(file);
            return -1;
        }
        if (control_count == 0)
            receiver_port = port;
        else if (port != receiver_port) {
            fprintf(stderr, "%s:%u: all controls must use UDP port %u\n",
                    path, line_number, receiver_port);
            fclose(file);
            return -1;
        }
        control->minimum = (float)minimum;
        control->maximum = (float)maximum;
        control->index = -1;
        ++control_count;
    }
    if (ferror(file)) {
        fprintf(stderr, "Unable to read OSC control config %s\n", path);
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

static int process(jack_nframes_t frames, void *argument)
{
    (void)argument;
    jack_default_audio_sample_t *left = jack_port_get_buffer(left_port, frames);
    jack_default_audio_sample_t *right = jack_port_get_buffer(right_port, frames);
    for (size_t i = 0; i < control_count; ++i) {
        if (controls[i].index < 0)
            continue;
        uintptr_t bits = atomic_load_explicit(&controls[i].bits,
                                              memory_order_acquire);
        dclang2c_set_float((unsigned int)controls[i].index,
                           bits_to_float(bits));
    }
    float interleaved[2 * frames];
    dclang2c_render(interleaved, frames);
    for (jack_nframes_t frame = 0; frame < frames; ++frame) {
        left[frame] = interleaved[2 * frame];
        right[frame] = interleaved[2 * frame + 1];
    }
    return 0;
}

static void jack_shutdown(void *argument)
{
    (void)argument;
    running = 0;
}

static void stop(int signal_number)
{
    (void)signal_number;
    running = 0;
}

int main(int argc, char **argv)
{
    if (argc > 3) {
        fprintf(stderr, "Usage: %s [JACK-client-name [OSC-control-config]]\n",
                argv[0]);
        return EXIT_FAILURE;
    }
    const char *name = argc >= 2 ? argv[1] : "dclang-dsp";
    const char *control_config = NULL;
#ifdef DCLANG_OSC_CONFIG
    control_config = DCLANG_OSC_CONFIG;
#endif
    if (argc == 3)
        control_config = argv[2];
    jack_status_t status;
    client = jack_client_open(name, JackNoStartServer, &status);
    if (!client) {
        fprintf(stderr, "Unable to open JACK client (status 0x%x)\n", status);
        return EXIT_FAILURE;
    }
    jack_nframes_t sample_rate = jack_get_sample_rate(client);
    if (sample_rate != DCLANG_LOCAL_SR) {
        fprintf(stderr,
                "JACK runs at %u Hz, but this instrument was generated for %u Hz\n",
                sample_rate, (unsigned int)DCLANG_LOCAL_SR);
        jack_client_close(client);
        return EXIT_FAILURE;
    }

    jack_set_process_callback(client, process, NULL);
    jack_on_shutdown(client, jack_shutdown, NULL);
    left_port = jack_port_register(client, "out_1", JACK_DEFAULT_AUDIO_TYPE,
                                   JackPortIsOutput, 0);
    right_port = jack_port_register(client, "out_2", JACK_DEFAULT_AUDIO_TYPE,
                                    JackPortIsOutput, 0);
    if (!left_port || !right_port) {
        fprintf(stderr, "Unable to register JACK output ports\n");
        jack_client_close(client);
        return EXIT_FAILURE;
    }

    dclang2c_init();
    if (load_controls(control_config) != 0) {
        jack_client_close(client);
        return EXIT_FAILURE;
    }
    size_t active_controls = 0;
    for (size_t i = 0; i < control_count; ++i) {
        controls[i].index = dclang2c_control_index(controls[i].name);
        if (controls[i].index < 0) {
            fprintf(stderr, "%s: generated DSP has no variable named %s\n",
                    control_config, controls[i].name);
            jack_client_close(client);
            return EXIT_FAILURE;
        }
        ++active_controls;
        atomic_store_explicit(&controls[i].bits,
                              float_to_bits(dclang2c_get_float(
                                  (unsigned int)controls[i].index)),
                              memory_order_relaxed);
    }
    if (active_controls && start_receiver() != 0) {
        fprintf(stderr, "Unable to start OSC receiver on UDP port %u: %s\n",
                receiver_port, strerror(errno));
        if (receiver_socket >= 0)
            close(receiver_socket);
        jack_client_close(client);
        return EXIT_FAILURE;
    }
    if (jack_activate(client) != 0) {
        fprintf(stderr, "Unable to activate JACK client\n");
        stop_receiver();
        jack_client_close(client);
        return EXIT_FAILURE;
    }

    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    fprintf(stderr, "%s: JACK ports %s and %s at %u Hz\n",
            jack_get_client_name(client), jack_port_name(left_port),
            jack_port_name(right_port), sample_rate);
    if (active_controls)
        fprintf(stderr, "%s: OSC controls on UDP port %u\n",
                jack_get_client_name(client), receiver_port);
    while (running)
        sleep(1);

    jack_deactivate(client);
    stop_receiver();
    jack_client_close(client);
    return EXIT_SUCCESS;
}

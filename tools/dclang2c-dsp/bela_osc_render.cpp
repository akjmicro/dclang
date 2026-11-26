#include <Bela.h>

#include <arpa/inet.h>
#include <atomic>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#ifndef DCLANG_OSC_PORT
#define DCLANG_OSC_PORT 7562
#endif

extern "C" {
void dclang2c_init(void);
void dclang2c_render(float *output, size_t frames);
int dclang2c_control_index(const char *name);
void dclang2c_set_u32(unsigned int index, uint32_t value);
void dclang2c_set_float(unsigned int index, float value);
}

struct ControlUpdate {
    unsigned int index;
    bool floating;
    uint32_t bits;
};

static constexpr size_t CONTROL_QUEUE_CAPACITY = 128;
static ControlUpdate controlQueue[CONTROL_QUEUE_CAPACITY];
static std::atomic<size_t> controlWrite{0};
static std::atomic<size_t> controlRead{0};

static std::vector<float> block;
static pthread_t receiverThread;
static std::atomic<bool> receiverRunning{false};
static int receiverSocket = -1;
static bool renderingStarted = false;

static size_t oscStringSize(const unsigned char *packet, size_t bytes,
                            size_t offset)
{
    if (offset >= bytes)
        return 0;
    const void *end = memchr(packet + offset, 0, bytes - offset);
    if (!end)
        return 0;
    size_t length = static_cast<const unsigned char *>(end) - packet - offset;
    size_t padded = (length + 1 + 3) & ~size_t(3);
    return padded <= bytes - offset ? padded : 0;
}

static bool enqueueControl(const ControlUpdate &control)
{
    const size_t write = controlWrite.load(std::memory_order_relaxed);
    const size_t next = (write + 1) % CONTROL_QUEUE_CAPACITY;
    if (next == controlRead.load(std::memory_order_acquire))
        return false;
    controlQueue[write] = control;
    controlWrite.store(next, std::memory_order_release);
    return true;
}

static void acceptPacket(const unsigned char *packet, size_t bytes)
{
    size_t pathSize = oscStringSize(packet, bytes, 0);
    if (!pathSize)
        return;
    size_t typeSize = oscStringSize(packet, bytes, pathSize);
    if (!typeSize)
        return;

    const char *path = reinterpret_cast<const char *>(packet);
    const char *types = reinterpret_cast<const char *>(packet + pathSize);
    if (strcmp(path, "/dclang/set") != 0)
        return;

    bool floating;
    if (strcmp(types, ",sf") == 0)
        floating = true;
    else if (strcmp(types, ",si") == 0)
        floating = false;
    else
        return;

    const size_t nameOffset = pathSize + typeSize;
    const size_t nameSize = oscStringSize(packet, bytes, nameOffset);
    if (!nameSize || nameOffset + nameSize + 4 > bytes)
        return;
    const char *name = reinterpret_cast<const char *>(packet + nameOffset);
    if (!*name)
        return;
    const int index = dclang2c_control_index(name);
    if (index < 0)
        return;

    uint32_t networkBits;
    memcpy(&networkBits, packet + nameOffset + nameSize, sizeof(networkBits));
    uint32_t bits = ntohl(networkBits);
    enqueueControl({static_cast<unsigned int>(index), floating, bits});
}

static bool applyPendingControls()
{
    bool applied = false;
    size_t read = controlRead.load(std::memory_order_relaxed);
    const size_t write = controlWrite.load(std::memory_order_acquire);
    while (read != write) {
        const ControlUpdate control = controlQueue[read];
        if (control.floating) {
            float value;
            memcpy(&value, &control.bits, sizeof(value));
            dclang2c_set_float(control.index, value);
        } else {
            dclang2c_set_u32(control.index, control.bits);
        }
        applied = true;
        read = (read + 1) % CONTROL_QUEUE_CAPACITY;
    }
    controlRead.store(read, std::memory_order_release);
    return applied;
}

static void *receiveOsc(void *)
{
    unsigned char packet[256];
    while (receiverRunning.load(std::memory_order_acquire)) {
        ssize_t bytes = recv(receiverSocket, packet, sizeof(packet), 0);
        if (bytes > 0)
            acceptPacket(packet, static_cast<size_t>(bytes));
        else if (bytes < 0 && errno != EINTR && receiverRunning.load())
            break;
    }
    return nullptr;
}

static bool startReceiver()
{
    receiverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiverSocket < 0)
        return false;
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(DCLANG_OSC_PORT);
    if (bind(receiverSocket, reinterpret_cast<sockaddr *>(&address),
             sizeof(address)) < 0) {
        close(receiverSocket);
        receiverSocket = -1;
        return false;
    }
    receiverRunning.store(true, std::memory_order_release);
    if (pthread_create(&receiverThread, nullptr, receiveOsc, nullptr) != 0) {
        receiverRunning.store(false);
        close(receiverSocket);
        receiverSocket = -1;
        return false;
    }
    return true;
}

bool setup(BelaContext *context, void *)
{
    if (context->audioOutChannels < 2)
        return false;
    block.resize(2 * context->audioFrames);
    dclang2c_init();
    controlRead.store(0, std::memory_order_relaxed);
    controlWrite.store(0, std::memory_order_relaxed);
    renderingStarted = false;
    return startReceiver();
}

void render(BelaContext *context, void *)
{
    if (applyPendingControls())
        renderingStarted = true;

    if (!renderingStarted) {
        for (unsigned int frame = 0; frame < context->audioFrames; ++frame) {
            audioWrite(context, frame, 0, 0.0f);
            audioWrite(context, frame, 1, 0.0f);
        }
        return;
    }

    dclang2c_render(block.data(), context->audioFrames);
    for (unsigned int frame = 0; frame < context->audioFrames; ++frame) {
        audioWrite(context, frame, 0, block[2 * frame]);
        audioWrite(context, frame, 1, block[2 * frame + 1]);
    }
}

void cleanup(BelaContext *, void *)
{
    receiverRunning.store(false, std::memory_order_release);
    if (receiverSocket >= 0) {
        shutdown(receiverSocket, SHUT_RDWR);
        close(receiverSocket);
        receiverSocket = -1;
        pthread_join(receiverThread, nullptr);
    }
}

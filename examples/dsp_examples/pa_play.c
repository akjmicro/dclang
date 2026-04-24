#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portaudio.h>

#define DEFAULT_SAMPLE_RATE 48000
#define DEFAULT_CHANNELS 2
#define DEFAULT_FRAMES_PER_BUFFER 512
#define DEFAULT_PREFILL_BUFFERS 4

typedef enum {
    FMT_S16LE,
    FMT_S24LE,
    FMT_S32LE,
    FMT_F32LE
} AudioFormat;

typedef struct {
    int sample_rate;
    int channels;
    int frames_per_buffer;
    int prefill_buffers;
    AudioFormat format;
    PaSampleFormat pa_format;
    size_t sample_size_bytes;
} Config;

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [--rate RATE] [--channels N] [--format FORMAT] [--buffer FRAMES] [--prefill N]\n"
        "  FORMAT: s16le (default), s24le, s32le, f32le\n",
        prog);
    exit(1);
}

int main(int argc, char *argv[]) {
    Config cfg = {
        .sample_rate = DEFAULT_SAMPLE_RATE,
        .channels = DEFAULT_CHANNELS,
        .frames_per_buffer = DEFAULT_FRAMES_PER_BUFFER,
        .prefill_buffers = DEFAULT_PREFILL_BUFFERS,
        .format = FMT_S32LE,
        .pa_format = paInt32,
        .sample_size_bytes = 2
    };

    for (int i = 1; i < argc; i++) {
        if ((!strcmp(argv[i], "--rate") || !strcmp(argv[i], "-r")) && i + 1 < argc) {
            cfg.sample_rate = atoi(argv[++i]);
        } else if ((!strcmp(argv[i], "--channels") || !strcmp(argv[i], "-c")) && i + 1 < argc) {
            cfg.channels = atoi(argv[++i]);
        } else if ((!strcmp(argv[i], "--buffer") || !strcmp(argv[i], "-b")) && i + 1 < argc) {
            cfg.frames_per_buffer = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--prefill") && i + 1 < argc) {
            cfg.prefill_buffers = atoi(argv[++i]);
        } else if ((!strcmp(argv[i], "--format") || !strcmp(argv[i], "-f")) && i + 1 < argc) {
            const char *fmt = argv[++i];
            if (!strcmp(fmt, "s16le")) {
                cfg.format = FMT_S16LE;
                cfg.pa_format = paInt16;
                cfg.sample_size_bytes = 2;
            } else if (!strcmp(fmt, "s24le")) {
                cfg.format = FMT_S24LE;
                cfg.pa_format = paInt24;
                cfg.sample_size_bytes = 3;
            } else if (!strcmp(fmt, "s32le")) {
                cfg.format = FMT_S32LE;
                cfg.pa_format = paInt32;
                cfg.sample_size_bytes = 4;
            } else if (!strcmp(fmt, "f32le")) {
                cfg.format = FMT_F32LE;
                cfg.pa_format = paFloat32;
                cfg.sample_size_bytes = 4;
            } else {
                fprintf(stderr, "Unsupported format: %s\n", fmt);
                usage(argv[0]);
            }
        } else {
            usage(argv[0]);
        }
    }

    const size_t block_bytes =
        (size_t)cfg.frames_per_buffer * cfg.channels * cfg.sample_size_bytes;

    const int prefill_blocks =
        (cfg.prefill_buffers < 1) ? 1 : cfg.prefill_buffers;

    unsigned char *prefill =
        malloc(block_bytes * (size_t)prefill_blocks);
    unsigned char *buffer =
        malloc(block_bytes);

    if (!prefill || !buffer) {
        perror("malloc");
        free(prefill);
        free(buffer);
        return 1;
    }

    size_t prefill_used = 0;
    int blocks_read = 0;

    // Read first chunk(s) BEFORE starting audio stream.
    while (blocks_read < prefill_blocks) {
        size_t n = fread(prefill + prefill_used, 1, block_bytes, stdin);
        if (n == 0) break;
        prefill_used += n;
        blocks_read++;
        if (n < block_bytes) break; // partial block / EOF
    }

    if (prefill_used == 0) {
        // No audio ever arrived; exit quietly.
        free(prefill);
        free(buffer);
        return 0;
    }

    PaStream *stream = NULL;
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) goto error;

    err = Pa_OpenDefaultStream(
        &stream,
        0,
        cfg.channels,
        cfg.pa_format,
        cfg.sample_rate,
        cfg.frames_per_buffer,
        NULL,
        NULL
    );
    if (err != paNoError) goto error;

    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;

    // Play the prefilled audio first.
    size_t offset = 0;
    while (offset < prefill_used) {
        size_t remaining = prefill_used - offset;
        size_t this_write = remaining > block_bytes ? block_bytes : remaining;
        int frames = (int)(this_write / (cfg.sample_size_bytes * cfg.channels));

        if (frames > 0) {
            err = Pa_WriteStream(stream, prefill + offset, frames);
            if (err != paNoError && err != paOutputUnderflowed) goto error;
            if (err == paOutputUnderflowed) {
                fprintf(stderr, "warning: output underflow\n");
            }
        }

        offset += this_write;
    }

    // Main loop
    while (1) {
        size_t n = fread(buffer, 1, block_bytes, stdin);
        if (n == 0) break;

        int frames = (int)(n / (cfg.sample_size_bytes * cfg.channels));
        if (frames <= 0) continue;

        err = Pa_WriteStream(stream, buffer, frames);
        if (err != paNoError && err != paOutputUnderflowed) goto error;
        if (err == paOutputUnderflowed) {
            fprintf(stderr, "warning: output underflow\n");
        }
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) goto error;

    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;

    Pa_Terminate();
    free(prefill);
    free(buffer);
    return 0;

error:
    fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    if (stream) {
        Pa_AbortStream(stream);
        Pa_CloseStream(stream);
    }
    Pa_Terminate();
    free(prefill);
    free(buffer);
    return 1;
}

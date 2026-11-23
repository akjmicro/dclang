#include <Bela.h>
#include <cstddef>
#include <vector>

extern "C" void dclang2c_init(void);
extern "C" void dclang2c_render(double *output, size_t frames);

static std::vector<double> block;

bool setup(BelaContext *context, void *)
{
    if (context->audioOutChannels < 2)
        return false;
    block.resize(2 * context->audioFrames);
    dclang2c_init();
    return true;
}

void render(BelaContext *context, void *)
{
    dclang2c_render(block.data(), context->audioFrames);
    for (unsigned int frame = 0; frame < context->audioFrames; ++frame) {
        audioWrite(context, frame, 0, block[2 * frame]);
        audioWrite(context, frame, 1, block[2 * frame + 1]);
    }
}

void cleanup(BelaContext *, void *)
{
}

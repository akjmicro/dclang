#include <math.h>
#include <stddef.h>

void dclang2c_init(void);
void dclang2c_render(float *output, size_t frames);
int dclang2c_control_index(const char *name);
double dclang2c_get_float(unsigned int index);
void dclang2c_set_float(unsigned int index, double value);

int main(void)
{
    float output[32];
    dclang2c_init();
    if (dclang2c_control_index("__missing_control__") != -1)
        return 1;
    int tempo = dclang2c_control_index("tempo");
    if (tempo >= 0) {
        dclang2c_set_float((unsigned int)tempo, 3.25);
        if (dclang2c_get_float((unsigned int)tempo) != 3.25)
            return 1;
    }
    dclang2c_render(output, 16);
    for (size_t i = 0; i < 32; ++i)
        if (!isfinite(output[i]))
            return 1;
    return output[2] == 0.0f && output[3] == 0.0f;
}

#include "portmidi.h"
#include "porttime.h"

// will be the private pointer to the working MIDI stream
PmStream *midi_stream;
#define TIME_PROC ((int32_t (*)(void *)) Pt_Time)
#define TIME_INFO NULL

void _pm_listfunc()
{
    /* list device information */
    int default_in = Pm_GetDefaultInputDeviceID();
    int default_out = Pm_GetDefaultOutputDeviceID();
    for (int i = 0; i < Pm_CountDevices(); i++) {
        char *deflt;
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        printf("%d: %s, %s", i, info->interf, info->name);
        if (info->input) {
            deflt = (i == default_in ? "default " : "");
            printf(" (%sinput)", deflt);
        }
        if (info->output) {
            deflt = (i == default_out ? "default " : "");
            printf(" (%soutput)", deflt);
        }
        printf("\n");
    }
}

void _pm_openoutfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'_pm_open_out' needs a device number on the stack!\n");
        return;
    }
    DCLANG_INT device = (DCLANG_INT) dclang_pop();
    Pm_OpenOutput(&midi_stream, device, NULL, 0, NULL, NULL , 0);
}

void _pm_wsfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("'_pm_ws' needs 3 integers on the stack:\n");
        printf("    <status> <data1> <data2>\n");
        return;
    }
    DCLANG_INT data2 = (DCLANG_INT) dclang_pop();
    DCLANG_INT data1 = (DCLANG_INT) dclang_pop();
    DCLANG_INT status = (DCLANG_INT) dclang_pop();
    Pm_WriteShort(
        midi_stream,
        TIME_PROC(TIME_INFO),
        Pm_Message(status, data1, data2)
    );
}

void _pm_wsrfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("'_pm_wsr' needs 3 integers on the stack:\n");
        printf("    <data2> <data1> <status>\n");
        return;
    }
    DCLANG_INT status = (DCLANG_INT) dclang_pop();
    DCLANG_INT data1 = (DCLANG_INT) dclang_pop();
    DCLANG_INT data2 = (DCLANG_INT) dclang_pop();
    Pm_WriteShort(
        midi_stream,
        0,
        Pm_Message(status, data1, data2)
    );
}

void _pm_closefunc()
{
    Pm_Close(midi_stream);
    printf("Portmidi port closed.\n");
}

void _pm_terminatefunc()
{
    Pm_Terminate();
    printf("Portmidi process terminated.\n");
}

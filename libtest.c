#include <stdio.h>
#include <dclang.h>

int import_ok;
int wordint;
double result;

int main()
{
    dclang_initialize();
    import_ok = dclang_import("libtest.dc");
    if (import_ok == -1) {
        printf("Import file not found, exiting!\n");
    }
    wordint = dclang_findword("libtest");
    if (wordint == -1) {
        printf("Word not found, exiting!\n");
        return -1;
    }
    dclang_callword((double) wordint);
    result = dclang_pop();
    printf("The result is %g\n", result);
}

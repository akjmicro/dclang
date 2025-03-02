#include "libdclang.c"

// Where all the juicy fun begins...
int main(int argc, char **argv)
{
    dclang_initialize();
    //setlocale(LC_ALL, "");
    /*
    if (argc > 1) {
        for(int opt = 1; opt < argc; opt++) {
            if (strcmp(argv[opt], "-i") == 0) {
                live_repl = 1;
            } else {
                dclang_import(argv[opt]);
            }
        };
    } else {
        live_repl = 1;
    }
    */
    live_repl = 1;
    if (live_repl) {
        printf("Welcome to dclang! Aaron Krister Johnson, 2018-2025\n");
        printf("Make sure to peruse README.md to get your bearings!\n");
        printf("You can type 'primitives' to see a list of all the primitive (c-builtin) words.\n");
        printf("You can type 'words' to see a list of functions defined within dclang.\n");
        show_primitivesfunc();
        repl();
    }
}

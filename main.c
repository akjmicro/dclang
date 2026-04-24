#include "libdclang.c"

// Where all the juicy fun begins...
int main(int argc, char **argv) {
    dclang_initialize();
    //setlocale(LC_ALL, "");
    if (argc > 1) {
        for(int opt = 1; opt < argc; opt++) {
            if (strcmp(argv[opt], "-i") == 0) {
                live_repl = 1;
            } else if (strcmp(argv[opt], "-e") == 0) {
                if (opt + 1 < argc) {
                    const char *code = argv[++opt];
                    FILE *f = fmemopen((void*)code, strlen(code), "r");
                    if (!f) {
                        perror("fmemopen failed to open input!");
                        return 1;
                    }
                    setinput(f);
                    repl();
                    fclose(f);
                } else {
                    fprintf(stderr, "dclang: -e requires an argument\n");
                    return 1;
                }
            } else {
                dclang_import(argv[opt]);
            }
        };
    } else {
        if (isatty(STDIN_FILENO)) {
            live_repl = 1;
        } else {
            repl();   // run interpreter on piped stdin
            return 0;
        }
    }

    if (live_repl) {
        setinput(stdin);
        printf("Welcome to dclang! (v5.2.2) Aaron Krister Johnson, 2018-2026\n");
        printf("Make sure to peruse README.md to get your bearings!\n");
        printf("You can type 'primitives' to see a list of all the primitive (c-builtin) words.\n");
        printf("You can type 'words' to see a list of words defined within dclang.\n");
        repl();
    }
}

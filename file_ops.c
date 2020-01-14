void fileopenfunc() {
    if (data_stack_ptr < 2) {
        printf("Stack underflow!\n");
        printf("'file-open' needs <filename> <open-mode> on the stack\n");
        return;
    }
    // file mode string
    char *mode = (char *)(unsigned long) pop();
    // file path
    char *path = (char *)(unsigned long) pop();
    FILE *openfptr;
    // if mode is read or append, file must exist:
    if ( (access(path, F_OK) == -1)
         && ( !strcmp("r", mode) || !strcmp("r+", mode) ) ) {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", path);
    }
    openfptr = fopen(path, mode);
    push((MYINT)openfptr);
}

void fileclosefunc() {
    if (data_stack_ptr < 1) {
        printf("Stack underflow!\n");
        printf("'file-close' needs <file-pointer> on the stack\n");
        return;
    }
    FILE *file_to_close = (FILE *)(long int) pop();
    fclose(file_to_close);
}

void filereadfunc() {
    if (data_stack_ptr < 2) {
        printf("Stack underflow!\n");
        printf("'file-read' needs <number-of-bytes> <file-pointer> on the stack\n");
        return;
    }
    FILE *file_to_read = (FILE *)(long int) pop();
    MYINT num_bytes = (MYINT) pop();
    char *buf = malloc(num_bytes);
    fread(buf, num_bytes, 1, file_to_read);
    // push the address of our new string and length
    push((MYINT)&buf);
}

void fileseekfunc() {
    if (data_stack_ptr < 3) {
        printf("Stack underflow!\n");
        printf("'file-seek' needs <offset> <whence> <file-pointer> on the stack\n");
        printf("'Whence' must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
        return;
    }
    FILE *file_to_seek = (FILE *)(long int) pop();
    MYINT whence = (MYINT) pop();
    if (!(whence >= 0 && whence <= 2)) {
        printf("Whence parameter must be between 0 and 2 inclusive!\n");
        return;
    }
    MYINT offset = (MYINT) pop();
    fseek(file_to_seek, offset, whence);
}

void filewritefunc() {
    if (data_stack_ptr < 2) {
        printf("Stack underflow!\n");
        printf("'file-write' needs <string-address> <file-pointer> on the stack\n");
        return;
    }
    FILE *file_to_write = (FILE *)(long int) pop();
    const void *str = (const void *)(unsigned long) pop();
    fwrite(str, 1, sizeof(str), file_to_write);
}

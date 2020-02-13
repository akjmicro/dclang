void fileopenfunc() {
    if (data_stack_ptr < 2) {
        printf("Stack underflow!\n");
        printf("'file-open' needs <filename> <open-mode> on the stack\n");
        return;
    }
    // file mode string
    char *mode = (char *)(MYUINT) pop();
    // file path
    char *path = (char *)(MYUINT) pop();
    // if mode is read or append, file must exist:
    if ( (access(path, F_OK) == -1)
         && ( !strcmp("r", mode) || !strcmp("r+", mode) ) ) {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", path);
    }
    FILE *openfptr = fopen(path, mode);
    push((MYUINT)openfptr);
}

void fileclosefunc() {
    if (data_stack_ptr < 1) {
        printf("Stack underflow!\n");
        printf("'file-close' needs <file-pointer> on the stack\n");
        return;
    }
    FILE *file_to_close = (FILE *)(MYUINT) pop();
    fclose(file_to_close);
}

void filereadfunc() {
    if (data_stack_ptr < 2) {
        printf("Stack underflow!\n");
        printf("'file-read' needs <number-of-bytes> <file-pointer> on the stack\n");
        return;
    }
    FILE *file_to_read = (FILE *)(MYUINT) pop();
    MYINT num_bytes = (MYUINT) pop();
    char *buf = malloc(num_bytes + 1);
    MYUINT num_bytes_read = fread(buf, num_bytes, 1, file_to_read);
    buf[num_bytes_read + 1] = "\0";
    // push the address of our new string and length
    push((MYUINT)buf);
}

void fileseekfunc() {
    if (data_stack_ptr < 3) {
        printf("Stack underflow!\n");
        printf("'file-seek' needs <offset> <whence> <file-pointer> on the stack\n");
        printf("'Whence' must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
        return;
    }
    FILE *file_to_seek = (FILE *)(MYUINT) pop();
    MYUINT whence = (MYUINT) pop();
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
    FILE *file_to_write = (FILE *)(MYUINT) pop();
    char *str = (char *)(MYUINT) pop();
    fwrite(str, 1, strlen(str), file_to_write);
    fflush(file_to_write);
}

void fileflushfunc()
{
    if (data_stack_ptr < 2) {
        printf("Stack underflow!\n");
        printf("'file-write' needs <string-address> <file-pointer> on the stack\n");
        return;
    }
    FILE *file_to_flush = (FILE *)(MYUINT) pop();
    fflush(file_to_flush);
}

void fileopenfunc() {
    if (data_stack_ptr < 4) {
        printf("Stack underflow!\n");
        printf("'file-open' needs <filename> <open-mode> on the stack\n");
        return;
    }
    // file mode string
    unsigned long str_len_mode = (unsigned long) pop();
    char *str_start_mode = (char *)((unsigned long) pop());
    char mode[str_len_mode + 1];
    char nullstr[] = "\0";
    memcpy(mode, (char *)str_start_mode, str_len_mode);
    memcpy(mode + str_len_mode, (char *)nullstr, 1);
    // file path
    unsigned long str_len_path = (unsigned long) pop();
    char *str_start_path = (char *)((unsigned long) pop());
    char filepath[str_len_path + 1];
    memcpy(filepath, (char *)str_start_path, str_len_path);
    memcpy(filepath + str_len_path, (char *)nullstr, 1);
    FILE *openfptr;
    // if mode is read or append, file must exist:
    if ( (access(filepath, F_OK) == -1)
         && ( !strcmp("r", mode) || !strcmp("r+", mode) ) ) {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", filepath);
    }
    openfptr = fopen(filepath, mode);
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
        printf("'file-read' needs <file-pointer> <number-of-bytes> on the stack\n");
        return;
    }
    MYINT num_bytes = (MYINT) pop();
    FILE *file_to_read = (FILE *)(long int) pop();
    char *local_buf = malloc(num_bytes);
    fread(local_buf, num_bytes, 1, file_to_read);
    // copy file contents to the string pad
    memcpy(&string_pad[string_here], local_buf, num_bytes);
    // push the address of our new string and length
    push((MYINT)&string_pad[string_here]);
    push(num_bytes);
    // update end of string pad:
    string_here += num_bytes;
    free(local_buf);
}

void fileseekfunc() {
    if (data_stack_ptr < 3) {
        printf("Stack underflow!\n");
        printf("'file-seek' needs <file-pointer> <offset> <whence> on the stack\n");
        printf("'Whence' must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
        return;
    }
    MYINT whence = (MYINT) pop();
    if (!(whence >= 0 && whence <= 2)) {
        printf("Whence parameter must be between 0 and 2 inclusive!\n");
        return;
    }
    MYINT offset = (MYINT) pop();
    FILE *file_to_seek = (FILE *)(long int) pop();
    fseek(file_to_seek, offset, whence);
}

void filewritefunc() {
    if (data_stack_ptr < 3) {
        printf("Stack underflow!\n");
        printf("'file-write' needs <file-pointer> <string-start> <string-length> on the stack\n");
        return;
    }
    MYINT strlen = (MYINT) pop();
    const void *strstart = (const void *)(long int) pop();
    FILE *file_to_write = (FILE *)(long int) pop();
    fwrite(strstart, 1, strlen, file_to_write);
}

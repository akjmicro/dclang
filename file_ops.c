void openfilefunc() {
    if (data_stack_ptr < 4) {
        printf("stack underflow! ");
    }
    // file mode string
    unsigned long str_len_mode = (unsigned long) pop();
    char *str_start_mode = (char *)((unsigned long) pop());
    char mode[str_len_mode + 1];
    char nullstr[] = "\0";
    memcpy(mode, (char *)(str_start_mode + 1), str_len_mode);
    memcpy(mode + str_len_mode, (char *)nullstr, 1);
    // file path
    unsigned long str_len_path = (unsigned long) pop();
    char *str_start_path = (char *)((unsigned long) pop());
    char filepath[str_len_path + 1];
    memcpy(filepath, (char *)(str_start_path + 1), str_len_path);
    memcpy(filepath + str_len_path - 1, (char *)nullstr, 1);
    printf("Filepath is %s\n", filepath);
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

void closefilefunc() {
    FILE *file_to_close = (FILE *)(long int)pop();
    fclose(file_to_close);
}

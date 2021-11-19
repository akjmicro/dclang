// used by 'freadline' function, which calls 'getline':
char *linebuf = NULL;
size_t linelen = 0;

static void fileopenfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("Stack underflow!\n");
        printf("'fopen' needs <filename> <open-mode> on the stack\n");
        return;
    }
    // file mode string
    char *mode = (char *)(DCLANG_UINT) pop();
    // file path
    char *path = (char *)(DCLANG_UINT) pop();
    // if mode is read or append, file must exist:
    if ( (access(path, F_OK) == -1)
         && ( !strcmp("r", mode) || !strcmp("r+", mode) ) )
    {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", path);
        return;
    }
    FILE *openfptr = fopen(path, mode);
    push((DCLANG_UINT)openfptr);
}

static void fileclosefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'fclose' needs <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_close = (FILE *)(DCLANG_UINT) pop();
    fclose(file_to_close);
}

static void filereadfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fread' needs <buf_pointer> <fpointer> <number-of-bytes> on the stack\n");
        return;
    }
    DCLANG_INT num_bytes = (DCLANG_UINT) pop();
    FILE *file_to_read = (FILE *)(DCLANG_UINT) pop();
    char *buf = (char *)(DCLANG_UINT) pop();
    DCLANG_UINT num_bytes_read = fread(buf, 1, num_bytes + 1, file_to_read);
    // update print safety:
    if ((DCLANG_UINT)buf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_UINT)buf;
    }
    if ((DCLANG_UINT)buf + num_bytes_read + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_UINT)buf + num_bytes_read + 1;
    }
    // push the address of the END of our new string
    push((DCLANG_UINT) buf + num_bytes_read);
    // push the number of bytes read
    push((DCLANG_INT) num_bytes_read);
}

static void filereadlinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'freadline' needs <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_read = (FILE *)(DCLANG_UINT) pop();
    DCLANG_UINT num_bytes_read = getline(&linebuf, &linelen, file_to_read);
    // update print safety:
    if ((DCLANG_UINT) linebuf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_UINT) linebuf;
    }
    if ((DCLANG_UINT) linebuf + num_bytes_read + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_UINT) linebuf + num_bytes_read + 1;
    }
    // push the address of our new string and length
    push((DCLANG_UINT) linebuf);
    // push the number of bytes read
    push((DCLANG_INT) num_bytes_read);
}

static void fileseekfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fseek' needs <offset> <whence> <fpointer> on the stack\n");
        printf("'Whence' must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
        return;
    }
    FILE *file_to_seek = (FILE *)(DCLANG_UINT) pop();
    DCLANG_UINT whence = (DCLANG_UINT) pop();
    if (!(whence >= 0 && whence <= 2))
    {
        printf("Whence parameter must be between 0 and 2 inclusive!\n");
        return;
    }
    DCLANG_INT offset = (DCLANG_INT) pop();
    fseek(file_to_seek, offset, whence);
}

static void filetellfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'ftell' needs a <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_tell = (FILE *)(DCLANG_UINT) pop();
    DCLANG_UINT mylen = ftell(file_to_tell);
    push((DCLANG_UINT) mylen);
}

static void filewritefunc()
{
    if (data_stack_ptr < 2)
    {
        printf("'fwrite' -- needs <string-address> <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_write = (FILE *)(DCLANG_UINT) pop();
    char *str = (char *)(DCLANG_UINT) pop();
    DCLANG_INT result = fwrite(str, 1, strlen(str), file_to_write);
    push(result);
}

static void fileflushfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'fflush' -- needs <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_flush = (FILE *)(DCLANG_UINT) pop();
    fflush(file_to_flush);
}

// lower-level OS calls:

static void openfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("Stack_underflow!\n");
        printf("'open' needs <filestr> <flagint> on the stack\n");
        return;
    }
    DCLANG_UINT flagint = (DCLANG_UINT) pop();
    char *path = (char *)(DCLANG_UINT)pop();
    int fd = open(path, flagint);
    push((DCLANG_UINT) fd);
}

static void readfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'read' needs <fpointer> <buffer-pointer> <numbytes> on the stack\n");
        return;
    }
    DCLANG_UINT numbytes = (DCLANG_UINT) pop();
    void *buf = (void *)(DCLANG_UINT)pop();
    int fd = (int) pop();
    int res = read(fd, buf, numbytes);
    push((int)res);
}

static void writefunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack_underflow!\n");
        printf("'write' needs <fpointer> <buffer-pointer> <numbytes> on the stack\n");
        return;
    }
    DCLANG_UINT numbytes = (DCLANG_UINT) pop();
    void *buf = (void *)(DCLANG_UINT)pop();
    int fd = (int) pop();
    int res = write(fd, buf, numbytes);
    push((int)res);
}

static void closefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'close' needs <fpointer> on the stack\n");
        return;
    }
    int fp = (int) pop();
    int res = close(fp);
    push(res);
}

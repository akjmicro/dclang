// used by 'freadline' function, which calls 'getline':
char *linebuf = NULL;
size_t linelen = 0;

void fileopenfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("Stack underflow!\n");
        printf("'fopen' needs <filename> <open-mode> on the stack\n");
        return;
    }
    // file mode string
    char *mode = (char *)(DCLANG_PTR) dclang_pop();
    // file path
    char *path = (char *)(DCLANG_PTR) dclang_pop();
    // if mode is read or append, file must exist:
    if ( (access(path, F_OK) == -1)
         && ( !strcmp("r", mode) || !strcmp("r+", mode) ) )
    {
        printf("The file named %s doesn't appear to exist, " \
               "or cannot be accessed.\n", path);
        return;
    }
    FILE *openfptr = fopen(path, mode);
    push((DCLANG_PTR)openfptr);
}

void fileclosefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'fclose' needs <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_close = (FILE *)(DCLANG_PTR) dclang_pop();
    fclose(file_to_close);
}

void filereadfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fread' needs <buf_pointer> <fpointer> <number-of-bytes> on the stack\n");
        return;
    }
    DCLANG_INT num_bytes = (DCLANG_PTR) dclang_pop();
    FILE *file_to_read = (FILE *)(DCLANG_PTR) dclang_pop();
    char *buf = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR num_bytes_read = fread(buf, 1, num_bytes + 1, file_to_read);
    // update print safety:
    if ((DCLANG_PTR)buf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_PTR)buf;
    }
    if ((DCLANG_PTR)buf + num_bytes_read + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_PTR)buf + num_bytes_read + 1;
    }
    // push the address of the END of our new string
    push((DCLANG_PTR) buf + num_bytes_read);
    // push the number of bytes read
    push((DCLANG_INT) num_bytes_read);
}

void filereadlinefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'freadline' needs <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_read = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR num_bytes_read = getline(&linebuf, &linelen, file_to_read);
    // update print safety:
    if ((DCLANG_PTR) linebuf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_PTR) linebuf;
    }
    if ((DCLANG_PTR) linebuf + num_bytes_read + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_PTR) linebuf + num_bytes_read + 1;
    }
    // push the address of our new string and length
    push((DCLANG_PTR) linebuf);
    // push the number of bytes read
    push((DCLANG_INT) num_bytes_read);
}

void filereadallfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'freadall' needs <fpointer> on the stack\n");
        return;
    }
    DCLANG_PTR chr_cnt = 0;
    DCLANG_PTR bufsize = 64;
    DCLANG_INT ch;
    char *allbuf = (char *) calloc(bufsize, sizeof(char));
    memset(allbuf, 0, bufsize);
    FILE *file_to_read = (FILE *)(DCLANG_PTR) dclang_pop();
    while ((ch = fgetc(file_to_read)) != EOF)
    {
        chr_cnt += 1;
        if (chr_cnt > bufsize)
        {
            bufsize *= 2;
            allbuf = (char *) realloc(allbuf, bufsize);
        }
        memset(allbuf + chr_cnt - 1, ch, 1);
    }
    memset(allbuf + chr_cnt, 0, 1);
    // update print safety:
    if ((DCLANG_PTR) allbuf < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = (DCLANG_PTR) allbuf;
    }
    if ((DCLANG_PTR) allbuf + chr_cnt + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = (DCLANG_PTR) allbuf + chr_cnt + 1;
    }
    // push the address of our new string and length
    push((DCLANG_PTR) allbuf);
    // push the number of bytes read
    push((DCLANG_INT) chr_cnt);
}

void fileseekfunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack underflow!\n");
        printf("'fseek' needs <offset> <whence> <fpointer> on the stack\n");
        printf("'Whence' must be 0 (SEEK_SET), 1 (SEEK_CUR), or 2 (SEEK_END).\n");
        return;
    }
    FILE *file_to_seek = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR whence = (DCLANG_PTR) dclang_pop();
    if (!(whence >= 0 && whence <= 2))
    {
        printf("Whence parameter must be between 0 and 2 inclusive!\n");
        return;
    }
    DCLANG_INT offset = (DCLANG_INT) dclang_pop();
    fseek(file_to_seek, offset, whence);
}

void filetellfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack underflow!\n");
        printf("'ftell' needs a <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_tell = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR mylen = ftell(file_to_tell);
    push((DCLANG_PTR) mylen);
}

void filewritefunc()
{
    if (data_stack_ptr < 3)
    {
        printf("'fwrite' -- needs <string-address> <num_of_bytes> <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_write = (FILE *)(DCLANG_PTR) dclang_pop();
    DCLANG_PTR num_bytes = (DCLANG_PTR) dclang_pop();
    char *str = (char *)(DCLANG_PTR) dclang_pop();
    DCLANG_INT result = fwrite(str, 1, num_bytes, file_to_write);
    push(result);
}

void fileflushfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("'fflush' -- needs <fpointer> on the stack\n");
        return;
    }
    FILE *file_to_flush = (FILE *)(DCLANG_PTR) dclang_pop();
    fflush(file_to_flush);
}

// lower-level OS calls:

void openfunc()
{
    if (data_stack_ptr < 2)
    {
        printf("Stack_underflow!\n");
        printf("'open' needs <filestr> <flagint> on the stack\n");
        return;
    }
    DCLANG_PTR flagint = (DCLANG_PTR) dclang_pop();
    char *path = (char *)(DCLANG_PTR)dclang_pop();
    int fd = open(path, flagint);
    push((DCLANG_PTR) fd);
}

void readfunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'read' needs <fpointer> <buffer-pointer> <numbytes> on the stack\n");
        return;
    }
    DCLANG_PTR numbytes = (DCLANG_PTR) dclang_pop();
    void *buf = (void *)(DCLANG_PTR)dclang_pop();
    int fd = (int) dclang_pop();
    int res = read(fd, buf, numbytes);
    push((int)res);
}

void writefunc()
{
    if (data_stack_ptr < 3)
    {
        printf("Stack_underflow!\n");
        printf("'write' needs <fpointer> <buffer-pointer> <numbytes> on the stack\n");
        return;
    }
    DCLANG_PTR numbytes = (DCLANG_PTR) dclang_pop();
    void *buf = (void *)(DCLANG_PTR)dclang_pop();
    int fd = (int) dclang_pop();
    int res = write(fd, buf, numbytes);
    push((int)res);
}

void closefunc()
{
    if (data_stack_ptr < 1)
    {
        printf("Stack_underflow!\n");
        printf("'close' needs <fpointer> on the stack\n");
        return;
    }
    int fp = (int) dclang_pop();
    int res = close(fp);
    push(res);
}

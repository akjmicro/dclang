#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Test that buffer reads never exceed declared length in dclang_import */
START_TEST(test_buffer_overflow_protection)
{
    /* Invariant: dclang_import must not read/write beyond 512-byte buffer
       when processing DCLANG_LIBS prefix + "/" + filename */
    
    const char *payloads[] = {
        "a",                                    /* valid: short filename */
        "normal_file.c",                        /* valid: typical filename */
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x" "x"
        ".c",                                   /* 256+ byte filename: overflow attempt */
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y" "y"
        ".c"                                    /* 512+ byte filename: severe overflow */
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        /* Set environment to trigger dclang_import path construction */
        setenv("DCLANG_LIBS", "/usr/lib/dclang

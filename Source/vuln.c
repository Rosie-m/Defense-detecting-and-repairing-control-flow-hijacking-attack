/* This program has a buffer overflow vulnerability. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// BUF_SIZE is the last two digits in your SFU ID.
// If the last two digits are 00, pick the next significant two digits
#ifndef BUF_SIZE
#define BUF_SIZE 20
#endif

int bof(char *str)
{
    char buffer[BUF_SIZE];
    printf("%p\n", __builtin_return_address(0));
    /* The following statement has a buffer overflow problem */
    strcpy(buffer, str);
    printf("%p\n", __builtin_return_address(0));
    return 1;
}

int main(int argc, char **argv)
{
    char str[517];
    FILE *badfile;
    char dummy[BUF_SIZE];
    memset(dummy, 0, BUF_SIZE);

    badfile = fopen("shellcode", "r");
    fread(str, sizeof(char), 517, badfile);
    bof(str);

    printf("Returned Properly\n");
    return 1;
}

/*
 * example programm that segfault's a lot.
 * $ gcc -Wall -o segfault segfault.c
 * $ LD_PRELOAD=./assfault.so ./segfault
 */
#include <stdio.h>
int
main()
{
    char *ptr=NULL;

    fprintf(stderr, "|0| everything looks fine. lets produce a SIGSEGV\n");
	*ptr=1;
	fprintf(stderr, "|1| after first provocated SIGSEGV\n");
    *ptr=1;
    fprintf(stderr, "|2| after second provocated SIGSEGV\n");
    fprintf(stderr, "|X| We survived - enough played today.\n");

    return 0;
}

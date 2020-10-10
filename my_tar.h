#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysmacros.h>

int my_strlen(char* p1)
{
    int i = 0;
    while(p1[i++] != '\0');
    return --i;
}

void my_strcpy(char* p1, char* p2)
{
    char *out = p1;
    while( (*p1++ = *p2++) );
    p1 = out;
}

void my_itoa(char* p1, long int p2, int base, int sign)
{
    unsigned long int p3;
    char out_0[16];
    char* out = &out_0[15];
    *out = '\0';

    if (p2 < 0 && sign == 1 )
        p3 = -p2;
    else if ( p2 < 0 )
        p3 = (unsigned int) p2;
    else
        p3 = p2;

    do
    {
        *--out = '0' + p3 % base;
        p3 /= base;
    }
    while ( p3 );

    my_strcpy(p1 , out);
}

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysmacros.h>
#include <dirent.h>

int my_strlen(char* p1)
{
    int i = 0;
    while(p1[i++] != '\0');
    return --i;
}

// void my_reverse_strcpy(char* p1, char* p2)
// {
//     int j = my_strlen(p1) - 1;
//     for (int i = my_strlen(p2) - 1; i >= 0; i--)
//         p1[j--] = p2[i];
// }

void my_strcpy(char* p1, char* p2)
{
    char *out = p1;
    while( (*p1++ = *p2++) );
    p1 = out;
}

void my_strcat(char* p1, char* p2)
{
    char* out = p1 + my_strlen(p1);
    while ( (*out++ = *p2++) );
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

int my_atoi(const char* input)
{
  int buffer = 0, cur_val = 0, sign = 1;
  size_t i = 0;

  if (input[0] == '-') {sign = -1; i++;}
  if (input[0] == '+') i++;

  while (input[i] != '\0') {
    curr_val = (int)(input[i] - '0');
    if (curr_val < 0 || curr_val > 9) return buffer;
    buffer = buffer * 10 + curr_val;
    i++;
  }

  return sign * buffer;
}

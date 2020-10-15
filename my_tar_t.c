#include "my_tar.h"

int my_tar_t (char* file_f)
{
    char buffer[20][100];
    int fd_file_f = open( file_f , O_RDONLY );
    int j = 0;

    while( read(fd_file_f, &buffer[j][0], 1) && buffer[j][0] != '\0')
    {
        char size[13];
        size[12] = '\0';
        int size_int;

        for (int i = 1; i < 100; i++)
        {
            read(fd_file_f, &buffer[j][i], 1);
        }

        lseek (fd_file_f, 24, SEEK_CUR);

        for (int i = 0; i < 12; i++)
        {
            read(fd_file_f, &size[i], 1);
        }

        size_int = my_oct_atoi(size);

        if ( (size_int % 512) == 0 )
            lseek (fd_file_f, 376 + size_int, SEEK_CUR);
        else
            lseek (fd_file_f, 376 + size_int + ( 512 - size_int % 512 ), SEEK_CUR);

        j++;
    }

    for (int i = 0; i < j; i++)
    {
        printf("%s\n", buffer[i]);
    }

    close( fd_file_f );

    return 0;
}

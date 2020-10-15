#include "my_tar.h"

int list_files (int fd_file_f, struct file_entry *buffer, int *count)
{
    while( read(fd_file_f, &buffer[*count].name[0], 1) && buffer[*count].name[0] != '\0')
    {
        int size_int;
        buffer[*count].size[12] = '\0';

        for (int i = 1; i < 100; i++)
            read(fd_file_f, &buffer[*count].name[i], 1);

        lseek (fd_file_f, 24, SEEK_CUR);

        for (int i = 0; i < 12; i++)
            read(fd_file_f, &buffer[*count].size[i], 1);

        for (int i = 0; i < 12; i++)
            read(fd_file_f, &buffer[*count].mtime[i], 1);

        size_int = my_oct_atoi(buffer[*count].size);

        if ( (size_int % 512) == 0 )
            lseek (fd_file_f, 364 + size_int, SEEK_CUR);
        else
            lseek (fd_file_f, 364 + size_int + ( 512 - size_int % 512 ), SEEK_CUR);

        (*count)++;
    }

    return 0;
}

int my_tar_t (char* file_f)
{
    struct file_entry buffer[20];
    int fd_file_f = open( file_f , O_RDONLY );
    int count = 0;

    list_files(fd_file_f, buffer, &count);

    close( fd_file_f );

    for (int i = 0; i < count; i++)
        printf("%s\n", buffer[i].name);

    return 0;
}

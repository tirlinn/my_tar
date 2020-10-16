#include "my_tar.h"

int my_tar_c (char* file_f, char** archive_files, int files_count)
{
    int fd_file_f = open( file_f , O_WRONLY | O_CREAT | O_TRUNC );

    for (int i = 0; i < files_count; i++)
    {
        write_file( fd_file_f, archive_files[i] );
    }

    write_null(fd_file_f, 1024);

    chmod(file_f, my_oct_atoi("777"));

    close( fd_file_f );
    return 0;
}

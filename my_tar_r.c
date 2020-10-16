#include "my_tar.h"

int my_tar_r (char* file_f, char** archive_files, int files_count)
{
    struct stat file_stat;

    int fd_file_f = open( file_f , O_WRONLY );

    fstat(fd_file_f, &file_stat);

    lseek(fd_file_f, file_stat.st_size - 1024, SEEK_SET);

    for (int i = 0; i < files_count; i++)
    {
        write_file( fd_file_f, archive_files[i] );
    }

    write_null(fd_file_f, 1024);

    close( fd_file_f );
    return 0;
}

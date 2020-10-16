#include "my_tar.h"

int is_fresh(char* archive_file, struct file_entry *buffer, int count, struct stat file_stat)
{
    int fd_file = open (archive_file, O_RDONLY);
    fstat(fd_file, &file_stat);

    for (int i = 0; i < count; i++)
    {
        if ( my_strcmp(archive_file, buffer[i].name) == 0 &&
             my_oct_atoi(buffer[i].mtime) >= file_stat.st_mtime )
        {
            close(fd_file);
            return 0;
        }
    }

    close(fd_file);
    return 1;
}

int my_tar_u(char* file_f, char** archive_files, int files_count)
{
    struct stat file_stat;
    struct file_entry buffer[20];

    int fd_file_f = open( file_f , O_RDWR );
    int count = 0;

    list_files(fd_file_f, buffer, &count);

    fstat(fd_file_f, &file_stat);

    lseek(fd_file_f, file_stat.st_size - 1024, SEEK_SET);

    for (int i = 0; i < files_count; i++)
        if ( is_fresh(archive_files[i], buffer, count) )
            write_file( fd_file_f, archive_files[i] );

    write_null(fd_file_f, 1024);

    close( fd_file_f );

    return 0;
}

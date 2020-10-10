#include "my_tar.h"

void archive_end (int fd_file_f)
{
    for (int i = 0; i < 1024; i++)
    {
        write(fd_file_f, "", 1);
    }
}

int my_tar_c (char* file_f, char** archive_files)
{
    int fd_file_f = open( file_f , O_WRONLY | O_CREAT | O_TRUNC );

    for (int i = 0; i < (sizeof(archive_files) / sizeof(archive_files[0])); i++) // sizeof / sizeof change for number of files to archive
    {
        write_file( fd_file_f, archive_files[i] );
    }

    archive_end(fd_file_f);

    close( fd_file_f );
    return 0;
}

int main()
{
    char* arr[10] = { "a" };
    my_tar_c("test", arr);
    return 0;
}

#include <stdio.h> 
#include <fcntl.h> 

void archive_end (int fd_file_f)
{
    for (int i = 0; i < 1024; i++)
    {
        write(fd_file_f, "", 1);
    }
}

int write_file ( char* file_f, char* archive_file )
{
    int fd_archive_file = open ( archive_file, O_RDONLY );
}

int my_tar_c (char* file_f, char** archive_files)
{
    int fd_file_f = open( file_f , O_WRONLY | O_CREAT | O_TRUNC );

    for (int i = 0; i < (sizeof(archive_files) / sizeof(archive_files[0])); i++)
    {
        write_file( fd_file_f, archive_files[i] );
    }

    archive_end(fd_file_f);

    close( fd_file_f );
    return 0;
}

int main()
{
    char** a = malloc(sizeof());
    
    my_tar_c("test", {"a","h"});
    return 0;
}
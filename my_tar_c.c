#include "my_tar.h"

int write_null (int fd_file_f, int size)
{
    char test[size];
    for(int i = 0; i < size; i++)
        test[i] = '\0';
    write(fd_file_f, test, size);
    return 0;
}

int my_tar_c (char* file_f, char** archive_files, int files_count)
{
    int fd_file_f = open( file_f , O_WRONLY | O_CREAT | O_TRUNC );

    for (int i = 0; i < files_count; i++)
    {
        write_file( fd_file_f, archive_files[i] );
    }

    write_null(fd_file_f, 1024);

    close( fd_file_f );
    return 0;
}

int main()
{
    char** arr = &argv[1];
    int i;
    for (i = 0; i < argc - 1; i++);
    my_tar_c("test", arr, i);
    return 0;


    "./my_tar -cf test.tar files"
    "./my_tar -f test.tar files -c"
    "./my_tar -cccccf test.tar files"
    "./my_tar -f test.tar files -c files"
    "./my_tar -crf test.tar files"
    "./my_tar -cf test.tar files -r files"
    "./my_tar -fx test.tar files -c "
}

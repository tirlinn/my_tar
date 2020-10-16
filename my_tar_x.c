#include "my_tar.h"

int read_file_header(int fd_file_f, struct posix_header *file_header)
{
    read(fd_file_f, file_header->name, 100);
    read(fd_file_f, file_header->mode, 8);
    read(fd_file_f, file_header->uid, 8);
    read(fd_file_f, file_header->gid, 8);
    read(fd_file_f, file_header->size, 12);
    read(fd_file_f, file_header->mtime, 12);
    read(fd_file_f, file_header->chksum, 8);
    char tmp[2];
    read(fd_file_f, tmp, 1);
    file_header->typeflag = tmp[0];
    read(fd_file_f, file_header->linkname, 100);
    read(fd_file_f, file_header->magic, 6);
    read(fd_file_f, file_header->version, 2);
    read(fd_file_f, file_header->uname, 32);
    read(fd_file_f, file_header->gname, 32);
    read(fd_file_f, file_header->devmajor, 8);
    read(fd_file_f, file_header->devminor, 8);
    read(fd_file_f, file_header->prefix, 155);
    lseek(fd_file_f, 12, SEEK_CUR);
    return 0;
}

int correct_stats(struct posix_header file_header)
{
    struct utimbuf times;
    times.actime = time('\0');
    times.modtime = my_oct_atoi(file_header.mtime);

    chmod(file_header.name, my_oct_atoi(file_header.mode));
    utime(file_header.name, &times);
    return 0;
}

int fill_content(int fd_archive_file, char* content_size, int fd_file_f)
{
    int size_int = my_oct_atoi(content_size);
    int cycles;

    if (size_int % 512 == 0)
        cycles = size_int/512;
    else
        cycles = size_int/512 + 1;

    for (int i = 0; i < cycles; i++)
    {
        char tmp[512];
        int blk_size;
        int check=0;
        if ( i == (cycles - 1) )
        {
            blk_size = size_int % 512;
            check = 1;
        }
        else
            blk_size = 512;
        read(fd_file_f, tmp, blk_size);
        write(fd_archive_file, tmp, blk_size);

        if(check == 1)
            lseek(fd_file_f, 512 - blk_size, SEEK_CUR);
    }

    return 0;
}

int my_tar_x(char* file_f)
{

    int fd_file_f = open( file_f , O_RDONLY );
    int fd_archive_file;
    char buffer[20];
    struct stat f_stat;

    while( read(fd_file_f, &buffer, 1) && buffer[0] != '\0' )
    {
        lseek(fd_file_f, -1, SEEK_CUR);

        struct posix_header file_header;

        read_file_header(fd_file_f, &file_header);

        if (file_header.typeflag == '5')
        {
            mkdir(file_header.name, my_oct_atoi(file_header.mode));
        }
        else
        {
            fd_archive_file = open( file_header.name , O_WRONLY | O_CREAT | O_TRUNC );

            fstat(fd_archive_file, &f_stat);

            correct_stats(file_header);

            fill_content(fd_archive_file, file_header.size, fd_file_f);

            close(fd_archive_file);
        }

    }

    close( fd_file_f );

    return 0;
}

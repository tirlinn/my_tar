#include "my_tar.h"

int read_file_header(int fd_file_f, struct posix_header *file_header)
{
    for (int i = 0; i < 100; i++)
        read(fd_file_f, file_header->name, 1);
    for (int i = 0; i < 8; i++)
        read(fd_file_f, file_header->mode, 1);
    for (int i = 0; i < 8; i++)
        read(fd_file_f, file_header->uid, 1);
    for (int i = 0; i < 8; i++)
        read(fd_file_f, file_header->gid, 1);
    for (int i = 0; i < 12; i++)
        read(fd_file_f, file_header->size, 1);
    for (int i = 0; i < 12; i++)
        read(fd_file_f, file_header->mtime, 1);
    for (int i = 0; i < 8; i++)
        read(fd_file_f, file_header->chksum, 1);
    char tmp[2];
    read(fd_file_f, tmp, 1);
    file_header->typeflag = tmp[0];
    for (int i = 0; i < 100; i++)
        read(fd_file_f, file_header->linkname, 1);
    for (int i = 0; i < 6; i++)
        read(fd_file_f, file_header->magic, 1);
    for (int i = 0; i < 2; i++)
        read(fd_file_f, file_header->version, 1);
    for (int i = 0; i < 32; i++)
        read(fd_file_f, file_header->uname, 1);
    for (int i = 0; i < 32; i++)
        read(fd_file_f, file_header->gname, 1);
    for (int i = 0; i < 8; i++)
        read(fd_file_f, file_header->devmajor, 1);
    for (int i = 0; i < 8; i++)
        read(fd_file_f, file_header->devminor, 1);
    for (int i = 0; i < 155; i++)
        read(fd_file_f, file_header->prefix, 1);
    lseek(fd_file_f, 12, SEEK_CUR);
    return 0;
}

int correct_stats(struct stat *f_stat, struct posix_header file_header)
{
    int tmp;
    switch (file_header.typeflag)
    {
        case '2':
            tmp = 40960;
            break;
        case '3':
            tmp = 8192;
            break;
        case '4':
            tmp = 24576;
            break;
        case '5':
            tmp = 16384;
            break;
        case '6':
            tmp = 4096;
            break;
        default:
            tmp = 32768;
            break;
    }

    f_stat->st_mode = tmp + my_oct_atoi(file_header.mode);
    f_stat->st_uid = my_oct_atoi(file_header.uid);
    f_stat->st_gid = my_oct_atoi(file_header.gid);
    f_stat->st_mtime = (long int) my_oct_atoi(file_header.mtime);

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
        if ( i == cycles - 1 )
            blk_size = size_int % 512;
        else
            blk_size = 512;
        read(fd_archive_file, tmp, blk_size);
        write(fd_archive_file, tmp, blk_size);
    }

    lseek (fd_file_f, size_int % 512, SEEK_CUR);

    return 0;
}

int my_tar_x(char* file_f)
{

    int fd_file_f = open( file_f , O_RDONLY );
    int fd_archive_file;
    char buffer[20];
    struct stat f_stat;

    while( read(fd_file_f, &buffer, 1) && buffer != '\0' )
    {
        printf("Tar X\n");
        lseek(fd_file_f, -1, SEEK_CUR);

        struct posix_header file_header;

        read_file_header(fd_file_f, &file_header);

        fd_archive_file = open( file_header.name , O_WRONLY | O_CREAT | O_TRUNC );

        fstat(fd_archive_file, &f_stat);

        correct_stats(&f_stat, file_header);

        fill_content(fd_archive_file, file_header.size, fd_file_f);

        close(fd_archive_file);
    }

    close( fd_file_f );

    return 0;
}

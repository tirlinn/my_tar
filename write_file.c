#include <my_tar.h>

struct posix_header
{                           /* byte offset */
    char name[100];         /*   0 */ //input
    char mode[8];           /* 100 */ //open -> fstat(id) -> st_mode ????
    char uid[8];            /* 108 */ //fstat -> user id
    char gid[8];            /* 116 */ //fstat -> group id
    char size[12];          /* 124 */ //fstat -> st_size
    char mtime[12];         /* 136 */ //fstat -> st_mtime
    char chksum[8];         /* 148 */ //???? seems to be some kind of algorithm to check the data archieved
    char typeflag;          /* 156 */ //???? https://en.wikipedia.o#includerg/wiki/Tar_(computing)
    char linkname[100];     /* 157 */ //For LNKTYPE and SYMTYPE
    char magic[6];          /* 257 */ //???? ustar\0 https://www.systutorials.com/docs/linux/man/5-tar/
    char version[2];        /* 263 */ //???? " \0"
    char uname[32];         /* 265 */ //fstat -> getpwuid
    char gname[32];         /* 297 */ //fstat -> getgrgid
    char devmajor[8];       /* 329 */ //major
    char devminor[8];       /* 337 */ //minor
    char prefix[155];       /* 345 */ //Name after 100 chars
                            /* 500 */ //12 empty bytes
};

char check_flag(struct stat file_stat)
{
    switch (file_stat.st_mode & S_IFMT)
    {
        case S_IFLNK:
            return '2';
        case S_IFCHR:
            return '3';
        case S_IFBLK:
            return '4';
        case S_IFDIR:
            return '5';
        case S_IFIFO:
            return '6';
        default:
            return '0';
    }
}

int count_sum(char* str)
{
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++)
        count += str[i];
    return count;
}

int check_sum(struct posix_header file_header)
{
    int count = count_sum(file_header.name)         +
                count_sum(file_header.mode)         +
                count_sum(file_header.uid)          +
                count_sum(file_header.gid)          +
                count_sum(file_header.size)         +
                count_sum(file_header.mtime)        +
                count_sum(file_header.linkname)     +
                count_sum(file_header.magic)        +
                count_sum(file_header.version)      +
                count_sum(file_header.uname)        +
                count_sum(file_header.gname)        +
                count_sum(file_header.devmajor)     +
                count_sum(file_header.devminor)     +
                count_sum(file_header.prefix)       +
                file_header.typeflag;
    return count;
}

int get_file_header(int fd_archive_file, char* archive_file, struct posix_header *file_header)
{
    struct stat file_stat;

    fstat(fd_archive_file, &file_stat);

    my_strcpy(file_header->name, archive_file);

    my_itoa(file_header->mode, file_stat.st_mode, 8, 1);
    file_header->mode[0] = '0';
    if (my_strlen(file_header->mode) == 6)
        file_header->mode[1] = '0';

    my_itoa(file_header->uid, file_stat.st_uid, 8, 1);
    my_itoa(file_header->gid, file_stat.st_gid, 8, 1);
    my_itoa(file_header->size, file_stat.st_size, 8, 1);
    my_itoa(file_header->mtime, file_stat.st_mtime, 8, 1);

    file_header->typeflag = check_flag(file_stat);

    if (file_header->typeflag == '2')
    {
        readlink(file_header->name, file_header->linkname, 100 );
    }

    my_strcpy(file_header->magic, "ustar"); // don't forget to write(x, " ", 1); at the end
    my_strcpy(file_header->version, " "); // don't forget

    struct passwd *user_name = getpwuid(file_stat.st_uid);
    my_strcpy(file_header->uname, user_name->pw_name);

    struct group *group_name = getgrgid(file_stat.st_gid);
    my_strcpy(file_header->gname, group_name->gr_name);

    if (file_header->typeflag == ('3'||'4'))
    {
        my_itoa(file_header->devmajor, major(file_stat.st_rdev), 8, 0);
        my_itoa(file_header->devminor, minor(file_stat.st_rdev), 8, 0);
    }

    my_itoa(file_header->chksum, check_sum(*file_header), 8, 0);

    return 0;
}

int write_header_entry(int fd_file_f, char* header_entry, int base, int is_numeric)
{
    if( is_numeric )
        write(fd_file_f, "00000000000", base - 1 - my_strlen(header_entry));

    write(fd_file_f, header_entry, my_strlen(header_entry));

    if( is_numeric )
        write_null(fd_file_f, 1);
    else
        write_null(fd_file_f, base - my_strlen(header_entry));
}

int write_file_header(int fd_file_f, struct posix_header file_header)
{
    write_header_entry(fd_file_f, file_header.name, 100, 0);
    write_header_entry(fd_file_f, file_header.mode, 8, 1);
    write_header_entry(fd_file_f, file_header.uid, 8, 1);
    write_header_entry(fd_file_f, file_header.gid, 8, 1);
    write_header_entry(fd_file_f, file_header.size, 12, 1);
    write_header_entry(fd_file_f, file_header.mtime, 12, 1);
    write_header_entry(fd_file_f, file_header.chksum, 8, 1);
    char tmp[] = {file_header.typeflag, '\0'};
    write(fd_file_f, tmp, 1);
    write_header_entry(fd_file_f, file_header.linkname, 100, 0);
    write(fd_file_f, file_header.magic, 5);
    write(fd_file_f, " ", 1);
    write(fd_file_f, file_header.version, 2);
    write_header_entry(fd_file_f, file_header.uname, 32, 0);
    write_header_entry(fd_file_f, file_header.gname, 32, 0);
    write_header_entry(fd_file_f, file_header.devmajor, 8, 0);
    write_header_entry(fd_file_f, file_header.devminor, 8, 0);
    write_header_entry(fd_file_f, file_header.prefix, 155, 0);
}

int write_file ( int fd_file_f, char* archive_file )
{
    struct posix_header file_header;

    int fd_archive_file = open ( archive_file, O_RDONLY );

    if (fd_archive_file < 0)
    {
        printf("Cannot open file %s.", archive_file);
        return -1;
    }

    get_file_header(fd_archive_file, archive_file, &file_header);
    write_file_header(fd_file_f, file_header);

    close(fd_archive_file);
}

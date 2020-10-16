#include "my_tar.h"

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
    if ( ( ( file_stat.st_mode & S_IFMT ) == S_IFDIR ) && file_header->name[my_strlen(file_header->name) - 1] != '/')
        my_strcat(file_header->name, "/");

    my_itoa(file_header->mode, file_stat.st_mode, 8, 1);
    file_header->mode[0] = '0';
    if (my_strlen(file_header->mode) == 6)
        file_header->mode[1] = '0';

    my_itoa(file_header->uid, file_stat.st_uid, 8, 1);
    my_itoa(file_header->gid, file_stat.st_gid, 8, 1);

    if ( ( file_stat.st_mode & S_IFMT ) != S_IFDIR )
        my_itoa(file_header->size, file_stat.st_size, 8, 1);
    else
        my_strcpy(file_header->size, "");

    my_itoa(file_header->mtime, file_stat.st_mtime, 8, 1);

    file_header->typeflag = check_flag(file_stat);

    if ((file_stat.st_mode & S_IFMT) == S_IFLNK)
        readlink(file_header->name, file_header->linkname, 100 );
    else
        my_strcpy(file_header->linkname, "");

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
    else
    {
        file_header->devmajor[0] = '\0';
        file_header->devminor[0] = '\0';
    }

    file_header->prefix[0] = '\0';

    my_itoa(file_header->chksum, check_sum(*file_header), 8, 0);

    return 0;
}

int write_null (int fd_file_f, int size)
{
    char test[size];
    for(int i = 0; i < size; i++)
        test[i] = '\0';
    write(fd_file_f, test, size);
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

    return 0;
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
    write_null(fd_file_f, 12);
    return 0;
}

int write_file_content ( int fd_file_f, int fd_archive_file, char* size)
{
    int size_int = my_oct_atoi(size);
    char file_content[size_int];
    int count = read(fd_archive_file, file_content, size_int);
    if (count > 0)
    {
        write(fd_file_f, file_content, count);
        write_null(fd_file_f, 512 - count % 512 );
    }
    return 0;
}

int write_link_content( int fd_file_f, char* archive_file, char* size)
{
    int size_int = my_oct_atoi(size);
    char link_content[size_int];
    int count = readlink(archive_file, link_content, size_int);
    if (count > 0)
    {
        write(fd_file_f, link_content, count);
        write_null(fd_file_f, 512 - count % 512 );
    }
    return 0;
}

int write_file ( int fd_file_f, char* archive_file )
{
    struct posix_header file_header;

    int fd_archive_file;

    fd_archive_file = open ( archive_file, O_RDONLY );

    if (fd_archive_file < 0)
    {
        printf("Cannot open file %s.\n", archive_file);
        return -1;
    }

    get_file_header(fd_archive_file, archive_file, &file_header);
    write_file_header(fd_file_f, file_header);

    if (file_header.typeflag == '2')
        write_link_content( fd_file_f, archive_file, file_header.size);
    else if (file_header.typeflag == '5')
    {
        DIR *folder;
        struct dirent *dir_entry;

        folder = opendir(archive_file);

        while ( (dir_entry = readdir(folder)) )
        {
            if (dir_entry->d_name[0] != '.')
            {
                int entry_length = my_strlen(archive_file) + my_strlen(dir_entry->d_name) + 2;
                char entry[entry_length];
                my_strcpy(entry, archive_file);
                my_strcat(entry, "/");
                my_strcat(entry, dir_entry->d_name);
                write_file(fd_file_f, entry);
            }
        }
        closedir(folder);
    }
    else
    {
        write_file_content( fd_file_f, fd_archive_file, file_header.size );
    }

    close(fd_archive_file);

    return 0;
}

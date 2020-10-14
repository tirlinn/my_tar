#include "my_tar.h"

struct posix_header
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
};

int write_null (int fd_file_f, int size)
{
    char test[size];
    for(int i = 0; i < size; i++)
        test[i] = '\0';
    write(fd_file_f, test, size);
    return 0;
}

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
    else if (file_header->typeflag == '5' && file_header->name[my_strlen(file_header->name) - 1] != '/')
    {
        my_strcat(file_header->name, "/");
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
    else
    {
        file_header->devmajor[0] = '\0';
        file_header->devminor[0] = '\0';
    }

    file_header->prefix[0] = '\0';

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
    int size_int = my_atoi(size);
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
    int size_int = my_atoi(size);
    char* link_content[size_int];
    int count = readlink(archive_file, link_content, size_int);
    if (count > 0)
    {
        write(fd_file_f, link_content, count);
        write_null(fd_file_f, 512 - count % 512 );
    }
    return 0;
}

// int write_dir_content( int fd_file_f, char* archive_file )
// {
//     DIR *folder;
//     folder = opendir(archive_file);
//     char* entry_name
//
//     while (entry_name = readdir(folder))
//     {
//         int entry_length = my_strlen(archive_file) + my_strlen(entry_name) + 1;
//         entry[entry_length];
//         my_strcpy(entry, archive_file);
//         my_strcat(entry, entry_name);
//         write_file(entry);
//     }
//
//     closedir(folder);
// }

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
        printf("Dir.");
        DIR *folder;
        struct dirent *dir_entry;

        folder = opendir(archive_file);
        printf("Dir opened.\n");

        while ( (dir_entry = readdir(folder)) )
        {
            if (dir_entry->d_name[0] != '.')
            {
                int entry_length = my_strlen(archive_file) + my_strlen(dir_entry->d_name) + 2;
                char entry[entry_length];
                my_strcpy(entry, archive_file);
                my_strcat(entry, "/");
                my_strcat(entry, dir_entry->d_name);
                printf("%s\n", entry);
                write_file(fd_file_f, entry);
            }
        }
        closedir(folder);
    }
    else
    {
        printf("Reched.");
        write_file_content( fd_file_f, fd_archive_file, file_header.size );
    }

    close(fd_archive_file);

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
    printf("Closed file.\n"); //DELETE
    return 0;
}

int run_option(char option, char* file_f, char** archive_files, int files_count)
{
    switch ( option )
    {
    case 'c':
        my_tar_c(file_f, archive_files, files_count);
        break;
    // case t:
    //     my_tar_t();
    //     break;
    // case r:
    //     my_tar_r();
    //     break;
    // case u:
    //     my_tar_u();
    //     break;
    // case x:
    //     my_tar_x();
    //     break;
    default:
        return 1;
        break;
    }

    return 0;
}

int format_error ( char option, char* file_f)
{
    if ( !option )
    {
        printf("You shall specify one of the -ctrux flags.\n");
        return 2;
    }
    else if ( option )
    {
        switch ( option )
        {
        case 'c': break;
        case 't': break;
        case 'r': break;
        case 'u': break;
        case 'x': break;
        default : return 3;
        }
    }

    if ( !file_f[0] ) //Not sure
    {
        printf("You need to use -f option.\n");
        return 4;
    }

    return 0;
}

int main (int argc, char** argv)
{
    char file_f[100];
    char* archive_files[10]; //We might need to malloc
    int files_count = 0, out;
    char option = '\0';

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            for (int j = 1; argv[i][j] != '\0'; j++)
            {
                if ( argv[i][j] == 'f' )
                {
                    if (argv[i][j + 1] != '\0')
                        my_strcpy(file_f, &argv[i][j + 1]); //Not sure
                    else
                    {
                        my_strcpy(file_f, argv[++i]);
                        break;
                    }
                }
                else if ( option == '\0')
                {
                    option = argv[i][j];
                }
                else if ( option != argv[i][j] )
                {
                    printf("You should use only one -ctrux option.\n");
                    return 1;
                }
            }
        }
        else
        {
            archive_files[files_count] = malloc(sizeof(char) * ( my_strlen(argv[i]) + 1) ); //Not sure
            my_strcpy (archive_files[files_count++], argv[i]);
        }
    }

    if ( ( out = format_error ( option, file_f ) ) != 0)
        return out;

    if ( ( out = run_option (option, file_f, archive_files, files_count) ) != 0 )
        return out;

    for (int i = 0; i < files_count; i++)
    {
        free(archive_files[i]);
    }

    return 0;
}

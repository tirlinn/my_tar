#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>

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

struct file_entry
{
    char name[100];
    char mtime[12];
    char size[12];
};

//my_tar.c
int format_error ( char option, char* file_f, int files_count );
int run_option(char option, char* file_f, char** archive_files, int files_count);
//my_tar_c.c
int my_tar_c (char* file_f, char** archive_files, int files_count);
//my_tar_t.c
int my_tar_t (char* file_f);
int list_files (int fd_file_f, struct file_entry *buffer, int *count);
//my_tar_r.c
int my_tar_r (char* file_f, char** archive_files, int files_count);
//my_tar_u.c
int is_fresh(char* archive_file, struct file_entry *buffer, int count);
int my_tar_u(char* file_f, char** archive_files, int files_count);
//my_tar_x.c
int correct_stats(struct posix_header file_header);
int fill_content(int fd_archive_file, char* content_size, int fd_file_f);
int my_tar_x(char* file_f);
//write_file.c
char check_flag(struct stat file_stat);
int count_sum(char* str);
int check_sum(struct posix_header file_header);
int get_file_header(int fd_archive_file, char* archive_file, struct posix_header *file_header);
int write_null (int fd_file_f, int size);
int write_file_header(int fd_file_f, struct posix_header file_header);
int write_file_content ( int fd_file_f, int fd_archive_file, char* size);
int write_link_content( int fd_file_f, char* archive_file, char* size);
int write_file ( int fd_file_f, char* archive_file );
//functions.c
int my_strlen(char* p1);
void my_strcpy(char* p1, char* p2);
void my_strcat(char* p1, char* p2);
int my_strcmp(char *p1, char *p2);
void my_itoa(char* p1, long int p2, int base, int sign);
int my_atoi(const char* input);
int my_oct_atoi(char* str);

struct posix_header
{                              /* byte offset */
    char name[100];               /*   0 */ input
    char mode[8];                 /* 100 */ open -> fstat(id) -> st_mode ????
    char uid[8];                  /* 108 */ fstat -> user id
    char gid[8];                  /* 116 */ fstat -> group id
    char size[12];                /* 124 */ fstat -> st_size
    char mtime[12];               /* 136 */ fstat -> st_mtime
    char chksum[8];               /* 148 */? seems to be some kind of algorithm to check the data archieved
    char typeflag;                /* 156 */? https://en.wikipedia.org/wiki/Tar_(computing)
    char linkname[100];           /* 157 */ For LNKTYPE and SYMTYPE
    char magic[6];                /* 257 */???? ustar\0 https://www.systutorials.com/docs/linux/man/5-tar/
    char version[2];              /* 263 */???? " \0"
    char uname[32];               /* 265 */ fstat -> getpwuid
    char gname[32];               /* 297 */ fstat -> getgrgid
    char devmajor[8];             /* 329 */ major
    char devminor[8];             /* 337 */ minor
    char prefix[155];             /* 345 */ Name after 100 chars
        /* 500 */ 12 empty bytes
}

struct stat
{
    dev_t     st_dev;         /* ID of device containing file */
    ino_t     st_ino;         /* Inode number */
    mode_t    st_mode;        /* File type and mode */
    nlink_t   st_nlink;       /* Number of hard links */
    uid_t     st_uid;         /* User ID of owner */
    gid_t     st_gid;         /* Group ID of owner */
    dev_t     st_rdev;        /* Device ID (if special file) */
    off_t     st_size;        /* Total size, in bytes */
    blksize_t st_blksize;     /* Block size for filesystem I/O */
    blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

    /* Since Linux 2.6, the kernel supports nanosecond
      precision for the following timestamp fields.
      For the details before Linux 2.6, see NOTES. */

    struct timespec st_atim;  /* Time of last access */
    struct timespec st_mtim;  /* Time of last modification */
    struct timespec st_ctim;  /* Time of last status change */

    #define st_atime st_atim.tv_sec      /* Backward compatibility */
    #define st_mtime st_mtim.tv_sec
    #define st_ctime st_ctim.tv_sec
};

write(2) 	+
malloc(3)	+
free(3) 	+
stat(2)		+
mkdir(2)	+
open(2)		+ https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
opendir		+
read(2)		+
readdir		+
chmod 		+
close|(2) +-
fstat(2) 	+-
getpwuid 	+-
getgrgid 	+-
symlink   +-
// getxattr 	-- not needed
// listxattr	-- not needed
lseek(2)	+- changes the pointer to the FD by the offset either relative or absolute
major		  +- fstat -> get device ID -> return major component
minor		  +- fstat -> get device ID -> return minor component
readlink	+- to read the content of the symbolic link
unlink(2) +- to delete the link
time 		  +- returns current time in seconds //time_t time( time_t *second )
ctime 		+- returns Www Mmm dd hh:mm:ss yyyy time as string //char *ctime(const time_t *timer)
utime 		+- changes modified time to the current time if times is NULL //int utime(const char *filename, const struct utimbuf *times);


-c create
-r add new
-t list
-u by time
-x extract
-f name of tar

chksum differs from original
struct doesn't get leading zeros that are counted by chksum

"./a.out  -cf" Works
"./a.out  -c" Works
"./a.out -a" Works
"./a.out -f" Works
"./a.out -ftest -c" Works
"./a.out -ftest -c file" Works
"./a.out -f test -c" Works
"./a.out -fc test a fee adir z" Works
"./a.out -f test a fee adir z" Works
"./a.out -f test -cf a fee adir z" Works
"./a.out -af test a fee adir z" Works

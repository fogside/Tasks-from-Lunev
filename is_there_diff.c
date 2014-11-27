#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
struct stat { // when _DARWIN_FEATURE_64_BIT_INODE is NOT defined //
         dev_t    st_dev;    // device inode resides on //
         ino_t    st_ino;    // inode's number //
         mode_t   st_mode;   // inode protection mode //
         nlink_t  st_nlink;  // number of hard links to the file //
         uid_t    st_uid;    // user-id of owner //
         gid_t    st_gid;    // group-id of owner //
         dev_t    st_rdev;   // device type, for special file inode //
         struct timespec st_atimespec;  // time of last access //
         struct timespec st_mtimespec;  // time of last data modification //
         struct timespec st_ctimespec;  // time of last file status change //
         off_t    st_size;   // file size, in bytes //
         quad_t   st_blocks; // blocks allocated for file //
         u_long   st_blksize;// optimal file sys I/O ops blocksize //
         u_long   st_flags;  // user defined flags for file //
         u_long   st_gen;    // file generation number //
     };

int  fstat(int fildes, struct stat *buf);

};*/

int Is_Diff(int fd1, int fd2)
{
    struct stat buf1, buf2;
    int st1 = fstat(fd1, &buf1);
    int st2 = fstat(fd2, &buf2);
    if((st1<0)||(st2<0))
        return -1;
     printf("INOD1: %llu, INOD2: %llu\n", buf1.st_ino, buf2.st_ino);
    if ((buf1.st_ino) == (buf2.st_ino))
        return 0;
    else return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 0;
    printf("%s\n", argv[1]);
    int fd1 = open(argv[1], O_RDWR);
    int fd2 = open(argv[1], O_RDWR);
    if ((fd1 != -1) && (fd2 != -1))
    {
        printf("fd1 = %d, fd2 = %d\n", fd1, fd2);
        printf("is there diff? -- %d\n", Is_Diff(fd1, fd2));
        return 0;
    }
    return 1;
}
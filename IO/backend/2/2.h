#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(include/syscall.h)
#include _WITCH_PATH(include/signal.h)

#if WITCH_LIBC
  /* we actually dont need this */
  /* but some retard standard libs need */
  #include <fcntl.h>
#else
  #include <asm/fcntl.h>
  #define SEEK_SET 0
  #define SEEK_CUR 1
  #define SEEK_END 2
  #define R_OK 4
  #define W_OK 2
  #define X_OK 1
  #define F_OK 0
#endif

#include <limits.h>

#define SF_NODISKIO 0x00000001
#define SF_MNOWAIT 0x00000002
#define SF_SYNC 0x00000004
#define SF_USER_READAHEAD 0x00000008
#define SF_NOCACHE 0x00000010

#define __S_IREAD 0400
#define __S_IWRITE 0200
#define __S_IEXEC 0100

typedef sint64_t IO_off_t;
typedef uintptr_t IO_size_t;
typedef sintptr_t IO_ssize_t;

typedef sint32_t IO_fd_t;
typedef sint32_t IO_dirfd_t;

PRE{
  signal(SIGPIPE, SIG_IGN);
}

struct freebsd11_stat{
  __uint32_t st_dev;
  __uint32_t st_ino;
  mode_t st_mode;
  __uint16_t st_nlink;
  uid_t st_uid;
  gid_t st_gid;
  __uint32_t st_rdev;
  struct timespec st_atim;
  struct timespec st_mtim;
  struct timespec st_ctim;
  off_t st_size;
  blkcnt_t st_blocks;
  blksize_t st_blksize;
  fflags_t st_flags;
  __uint32_t st_gen;
  __int32_t st_lspare;
  struct timespec st_birthtim;
  unsigned int :(8 / 2) * (16 - (int)sizeof(struct timespec));
  unsigned int :(8 / 2) * (16 - (int)sizeof(struct timespec));
};
typedef struct freebsd11_stat IO_stat_t;

static sint32_t IO_stat(const void *path, IO_stat_t *s){
  return -syscall2_noerr(SYS_freebsd11_stat, path, s);
}
static sint32_t IO_fstat(IO_fd_t fd, IO_stat_t *s){
  return -syscall2_noerr(SYS_freebsd11_fstat, fd, s);
}

static IO_off_t IO_stat_GetSizeInBytes(IO_stat_t *s){
  return s->st_size;
}

static bool IO_safepath(const char *path){
  bool check = 1;
  while(*path){
    if(check && *path == '.'){
      return 0;
    }
    check = *path == '/';
    path++;
  }
  return 1;
}

static bool IO_safepathn(const uint8_t *path, uintptr_t size){
  bool check = 1;
  for(uintptr_t i = 0; i < size; i++){
    if(check && path[i] == '.'){
      return 0;
    }
    check = path[i] == '/';
  }
  return 1;
}

static sint32_t IO_pipe(IO_fd_t *fds, uint32_t flag){
  return -syscall2_noerr(SYS_pipe2, fds, flag);
}

static IO_fd_t IO_open(const char *path, uint32_t flag){
  bool carry;
  sint32_t r = syscall3_carry(&carry, SYS_open, path, flag, __S_IREAD | __S_IWRITE);
  if(carry){
    r = -r;
  }
  return r;
}

static IO_fd_t IO_openat(IO_dirfd_t fd, const char *path, uint32_t flag){
  bool carry;
  sint32_t r = syscall4_carry(&carry, SYS_openat, fd, path, flag, __S_IREAD | __S_IWRITE);
  if(carry){
    r = -r;
  }
  return r;
}

static IO_fd_t IO_openatn(IO_dirfd_t fd, const void *ppath, uintptr_t pathsize, int flag){
  const char *path = (const char *)ppath;
  char npath[PATH_MAX];
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return IO_openat(fd, npath, flag);
}

static sint32_t IO_close(IO_fd_t fd){
  return -syscall1_noerr(SYS_close, fd);
}

static sint32_t IO_truncate(IO_fd_t fd, IO_off_t size){
  #if SYSTEM_BYTE == 8
    return -syscall2_noerr(SYS_ftruncate, fd, size);
  #elif SYSTEM_BYTE == 4
    uintptr_t u32[2] = {arg_u32e(size)};
    return -syscall3_noerr(SYS_ftruncate, fd, u32[0], u32[1]);
  #else
    #error ?
  #endif
}

static IO_off_t IO_lseek(IO_fd_t fd, IO_off_t isize, sint32_t state){
  #if SYSTEM_BYTE == 8
    bool carry;
    sintptr_t r = syscall3_carry(&carry, SYS_lseek, fd, isize, state);
    if(carry){
      r = -r;
    }
    return r;
  #elif SYSTEM_BYTE == 4
    uintptr_t u32[2] = {arg_u32e(isize)};
    bool carry;
    sintptr_t lo;
    sintptr_t hi = syscall4_extra_carry(&lo, &carry, SYS_lseek, fd, u32[1], u32[0], state);
    if(carry){
      return -hi;
    }
    return (IO_off_t)hi << 32 | lo;
  #else
    #error ?
  #endif
}

IO_off_t IO_ltell(IO_fd_t fd){
  return IO_lseek(fd, 0, SEEK_CUR);
}

static IO_ssize_t IO_write(IO_fd_t fd, const void *data, IO_size_t size){
  bool carry;
  IO_ssize_t r = syscall3_carry(&carry, SYS_write, fd, data, size);
  if(carry){
    if(r == EAGAIN){
      return 0;
    }
    r = -r;
  }
  return r;
}
static IO_ssize_t IO_read(IO_fd_t fd, void *data, IO_size_t size){
  bool carry;
  IO_ssize_t r = syscall3_carry(&carry, SYS_read, fd, data, size);
  if(carry){
    if(r == EAGAIN){
      return 0;
    }
    r = -r;
  }
  else if(r == 0){
    return -ESHUTDOWN;
  }
  return r;
}

static IO_ssize_t IO_pread(IO_fd_t fd, void *data, IO_off_t isize, IO_size_t nsize){
  IO_ssize_t r;
  bool carry;
  #if SYSTEM_BYTE == 8
    r = syscall4_carry(&carry, SYS_pread, fd, data, nsize, isize);
  #elif SYSTEM_BYTE == 4
    uintptr_t u32[2] = {arg_u32e(isize)};
    r = syscall5_carry(&carry, SYS_pread, fd, data, nsize, u32[0], u32[1]);
  #else
    #error ?
  #endif
  if(carry){
    if(r == EAGAIN){
      return 0;
    }
    r = -r;
  }
  else if(r == 0){
    return -ESHUTDOWN;
  }
  return r;
}

static IO_off_t IO_sendfile(IO_fd_t in, IO_fd_t out, IO_off_t isize, IO_off_t nsize){
  #if SYSTEM_BYTE == 4
    nsize = nsize > 0xffffffff ? 0xffffffff : nsize;
  #endif
  IO_off_t r;
  IO_ssize_t err = syscall7_noerr(SYS_sendfile, in, out, isize, nsize, 0, &r, 0);
  switch(err){
    case 0:
    case EAGAIN:
      return r;
    default:
      return -err;
  }
}

static sint32_t IO_rename(const void *src, const void *dst){
  return -syscall2_noerr(SYS_rename, src, dst);
}

static sint32_t IO_access(const void *path){
  return -syscall2_noerr(SYS_access, path, F_OK);
}

static void _IO_internal_open(){}
static void _IO_internal_close(){}

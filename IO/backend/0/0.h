#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(include/syscall.h)
#include _WITCH_PATH(include/signal.h)

#if WITCH_LIBC
  /* we actually dont need this */
  /* but some retard standard libs need */
  #include <fcntl.h>
  #include <sys/mman.h>
  typedef struct stat64 IO_stat_t;
#else
  #include <asm/fcntl.h>
  #include <asm/stat.h>
  #define SEEK_SET 0
  #define SEEK_CUR 1
  #define SEEK_END 2
  #define R_OK 4
  #define W_OK 2
  #define X_OK 1
  #define F_OK 0
  #if defined(__x86_64__)
    typedef struct stat IO_stat_t;
  #elif defined(__i386__)
    typedef struct stat64 IO_stat_t;
  #else
    #error ?
  #endif

  #ifndef MAP_SHARED
    #define MAP_SHARED 0x01
  #endif
  #ifndef MAP_PRIVATE
    #define MAP_PRIVATE 0x02
  #endif
  #ifndef MAP_SHARED_VALIDATE
    #define MAP_SHARED_VALIDATE 0x03
  #endif
#endif

#include <limits.h>

#define __S_IREAD 0400
#define __S_IWRITE 0200
#define __S_IEXEC 0100

typedef sint64_t IO_off_t;
typedef uintptr_t IO_size_t;
typedef sintptr_t IO_ssize_t;

typedef struct{
  sint32_t fd;
}IO_fd_t;
typedef struct{
  sint32_t fd;
}IO_dirfd_t;

#ifndef WITCH_PRE_is_not_allowed
  PRE{
    signal(SIGPIPE, SIG_IGN);
  }
#else
  void IO_init(void){
    signal(SIGPIPE, SIG_IGN);
  }
#endif

static void IO_fd_set(IO_fd_t *fd, sint32_t description){
  fd->fd = description;
}

static sint32_t IO_stat(const void *path, IO_stat_t *s){
  #ifdef __NR_stat64
    return syscall2(__NR_stat64, (uintptr_t)path, (uintptr_t)s);
  #else
    return syscall2(__NR_stat, (uintptr_t)path, (uintptr_t)s);
  #endif
}
static sint32_t IO_fstat(const IO_fd_t *fd, IO_stat_t *s){
  #ifdef __NR_fstat64
    return syscall2(__NR_fstat64, fd->fd, (uintptr_t)s);
  #else
    return syscall2(__NR_fstat, fd->fd, (uintptr_t)s);
  #endif
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

static sint32_t IO_fcntl(IO_fd_t *fd, sint32_t cmd, sint32_t arg){
  return syscall3(__NR_fcntl, fd->fd, cmd, arg);
}

static sint32_t IO_open(const void *path, uint32_t flag, IO_fd_t *fd){
  #if SYSTEM_BYTE < 8
    flag |= O_LARGEFILE;
  #endif
  sint32_t r = syscall3(__NR_open, (uintptr_t)path, flag, __S_IREAD | __S_IWRITE);
  if(r < 0){
    return r;
  }
  fd->fd = r;
  return 0;
}

static sint32_t IO_openat(const IO_dirfd_t *dirfd, const void *path, uint32_t flag, IO_fd_t *fd){
  #if SYSTEM_BYTE < 8
    flag |= O_LARGEFILE;
  #endif
  sint32_t r = syscall4(__NR_openat, dirfd->fd, (uintptr_t)path, flag, __S_IREAD | __S_IWRITE);
  if(r < 0){
    return r;
  }
  fd->fd = r;
  return 0;
}

static sint32_t IO_openatn(const IO_dirfd_t *dirfd, const void *ppath, uintptr_t pathsize, uint32_t flag, IO_fd_t *fd){
  const char *path = (const char *)ppath;
  char npath[PATH_MAX];
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return IO_openat(dirfd, npath, flag, fd);
}

static sint32_t IO_close(const IO_fd_t *fd){
  return syscall1(__NR_close, fd->fd);
}

static sint32_t IO_pipe(IO_fd_t *fds, IO_pipe_Flag Flag){
  bool fnr = !!(Flag & IO_pipe_Flag_NonblockRead);
  bool fnw = !!(Flag & IO_pipe_Flag_NonblockWrite);

  sintptr_t InternalFlag = 0;
  if(fnr){
    InternalFlag |= O_NONBLOCK;
  }
  if(Flag & IO_pipe_Flag_Packet){
    InternalFlag |= O_DIRECT;
  }
  sint32_t r = syscall2(__NR_pipe2, (uintptr_t)fds, InternalFlag);
  if(r != 0){
    return r;
  }
  if(fnr != fnw){
    r = IO_fcntl(&fds[1], F_SETFD, O_NONBLOCK * fnw);
    if(r != 0){
      IO_close(&fds[1]);
      IO_close(&fds[0]);
      return r;
    }
  }
  return 0;
}

static sint32_t IO_truncate(const IO_fd_t *fd, IO_off_t size){
  #if SYSTEM_BYTE == 8
    return syscall2(__NR_ftruncate, fd->fd, size);
  #elif SYSTEM_BYTE == 4
    uintptr_t u32[2] = {arg_u32e(size)};
    #if defined(__arm__)
      return syscall4(__NR_ftruncate64, fd->fd, 0, u32[0], u32[1]);
    #elif defined(__i386__)
      return syscall3(__NR_ftruncate64, fd->fd, u32[0], u32[1]);
    #else
      #error ?
    #endif
  #else
    #error ?
  #endif
}

static IO_off_t IO_lseek(const IO_fd_t *fd, IO_off_t isize, int state){
  #if SYSTEM_BYTE == 8
    return syscall3(__NR_lseek, fd->fd, isize, state);
  #elif SYSTEM_BYTE == 4
    uintptr_t u32[2] = {arg_u32e(isize)};
    IO_off_t r;
    /* u32 is swapped somehow i wonder reason too */
    sint32_t err = syscall5(__NR__llseek, fd->fd, u32[1], u32[0], (uintptr_t)&r, state);
    if(err){
      r = err;
    }
    return r;
  #else
    #error ?
  #endif
}

static IO_off_t IO_ltell(const IO_fd_t *fd){
  return IO_lseek(fd, 0, SEEK_CUR);
}

static IO_ssize_t IO_write(const IO_fd_t *fd, const void *data, IO_size_t size){
  IO_ssize_t r = syscall3(__NR_write, fd->fd, (uintptr_t)data, size);
  if(r < 0){
    if(r == -EAGAIN){
      return 0;
    }
  }
  return r;
}
static IO_ssize_t IO_read(const IO_fd_t *fd, void *data, IO_size_t size){
  IO_ssize_t r = syscall3(__NR_read, fd->fd, (uintptr_t)data, size);
  switch(r){
    case 0:{
      return -ESHUTDOWN;
    }
    case -EAGAIN:{
      return 0;
    }
  }
  return r;
}

static IO_ssize_t IO_pread(const IO_fd_t *fd, void *data, IO_off_t isize, IO_size_t nsize){
  IO_ssize_t r;
  #if SYSTEM_BYTE == 8
    r = syscall4(__NR_pread64, fd->fd, (uintptr_t)data, nsize, isize);
  #elif SYSTEM_BYTE == 4
    uintptr_t u32[2] = {arg_u32e(isize)};
    #if defined(__arm__)
      r = syscall6(__NR_pread64, fd->fd, (uintptr_t)data, nsize, 0, u32[0], u32[1]);
    #elif defined(__i386__)
      r = syscall5(__NR_pread64, fd->fd, (uintptr_t)data, nsize, u32[0], u32[1]);
    #else
      #error ?
    #endif
  #else
    #error ?
  #endif
  switch(r){
    case 0:{
      return -ESHUTDOWN;
    }
    case -EAGAIN:{
      return 0;
    }
  }
  return r;
}

static IO_off_t IO_sendfile(const IO_fd_t *in, const IO_fd_t *out, IO_off_t isize, IO_off_t nsize){
  nsize = nsize > 0x7ffff000 ? 0x7ffff000 : nsize;
  #ifdef __NR_sendfile64
    return syscall4(__NR_sendfile64, out->fd, in->fd, (uintptr_t)&isize, nsize);
  #else
    return syscall4(__NR_sendfile, out->fd, in->fd, (uintptr_t)&isize, nsize);
  #endif
}

static sint32_t IO_rename(const void *src, const void *dst){
  return syscall2(__NR_rename, (uintptr_t)src, (uintptr_t)dst);
}

static sint32_t IO_access(const void *path){
  return syscall2(__NR_access, (uintptr_t)path, F_OK);
}

static bool IO_IsPathExists(const void *path){
  return IO_access(path) >= 0;
}

static int IO_epoll_create(int flags){
  return syscall1(__NR_epoll_create1, flags);
}

static sintptr_t IO_mmap(void *addr, IO_size_t length, int prot, int flags, int fd, IO_off_t offset){
  return syscall6(__NR_mmap, (uintptr_t)addr, length, prot, flags, fd, offset);
}
static void IO_munmap(void *addr, IO_size_t length){
  int r = syscall2(__NR_munmap, (uintptr_t)addr, length);
  if(r != 0){
    PR_abort();
  }
}

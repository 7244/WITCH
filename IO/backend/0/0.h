#define _IO_RAWFD_STDIN 0
#define _IO_RAWFD_STDOUT 1
#define _IO_RAWFD_STDERR 2

#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(include/syscall.h)

#if !defined(__WITCH_IO_allow_sigpipe)
  #include _WITCH_PATH(include/signal.h)
#endif

#if defined(__platform_libc)
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

  /* PATH_MAX here */
  #include <linux/limits.h>
#endif

#include <limits.h>

#define __S_IREAD 0400
#define __S_IWRITE 0200
#define __S_IEXEC 0100

#include <linux/module.h>

typedef sint64_t IO_off_t;
typedef uintptr_t IO_size_t;
typedef sintptr_t IO_ssize_t;

typedef struct{
  sint32_t fd;
}IO_fd_t;
typedef struct{
  sint32_t fd;
}IO_dirfd_t;


static void IO_fd_set(IO_fd_t *fd, sint32_t description){
  fd->fd = description;
}
static sint32_t _IO_fd_get_internal(IO_fd_t *fd){
  return fd->fd;
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

static sint32_t IO_openatn(const IO_dirfd_t *dirfd, const void *path, uintptr_t pathsize, uint32_t flag, IO_fd_t *fd){
  char npath[PATH_MAX];
  __builtin_memcpy(npath, path, pathsize);
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
  gt_begin:;

  IO_ssize_t r = syscall3(__NR_write, fd->fd, (uintptr_t)data, size);
  if(r < 0){
    if(r == -EAGAIN){
      return 0;
    }
    if(r == -ENOBUFS){
      __processor_relax();
      goto gt_begin;
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

static bool IO_IsPathExists_cstr(const char *path){
  return IO_access(path) == 0;
}

static int IO_epoll_create(int flags){
  return syscall1(__NR_epoll_create1, flags);
}

static sintptr_t IO_ctl3(IO_fd_t *fd, uint32_t op, void *val){
  return syscall3(__NR_ioctl, fd->fd, op, (uintptr_t)val);
}
static sintptr_t IO_ctl2(IO_fd_t *fd, uint32_t op){
  return syscall2(__NR_ioctl, fd->fd, op);
}

static sintptr_t IO_mmap(void *addr, IO_size_t length, int prot, int flags, int fd, IO_off_t offset){
  return syscall6(__NR_mmap, (uintptr_t)addr, length, prot, flags, fd, offset);
}
static void IO_munmap(void *addr, IO_size_t length){
  sintptr_t r = syscall2(__NR_munmap, (uintptr_t)addr, length);
  if(r != 0){
    __abort();
  }
}

static sintptr_t IO_readlink_cstr(const char *path, uint8_t *out, uintptr_t out_size){
  return syscall3(__NR_readlink, (uintptr_t)path, (uintptr_t)out, out_size);
}

#define IO_QuickFileReadData_cstr(name_cstr, variable_name, buffer_size, ...) \
  uint8_t variable_name##_data[buffer_size]; \
  uintptr_t variable_name##_data_size; \
  do{ \
    IO_fd_t _IO_QuickFileReadData_cstr_fd; \
    sint32_t _IO_QuickFileReadData_cstr_err = IO_open((name_cstr), O_RDONLY, &_IO_QuickFileReadData_cstr_fd); \
    if(_IO_QuickFileReadData_cstr_err){ \
      variable_name##_data_size = (uintptr_t)_IO_QuickFileReadData_cstr_err; \
      break; \
    } \
    variable_name##_data_size = (uintptr_t)IO_read(&_IO_QuickFileReadData_cstr_fd, variable_name##_data, (buffer_size)); \
    if(variable_name##_data_size == (buffer_size)){ \
      variable_name##_data_size = (uintptr_t)-EFBIG; \
    } \
    IO_close(&_IO_QuickFileReadData_cstr_fd); \
  }while(0); \
  if(variable_name##_data_size > (uintptr_t)-0x1000){ \
    __VA_ARGS__ \
  }

#define IO_QuickExistingFileWriteData_cstr(name_cstr, variable_name, buffer_size, ...) \
  uintptr_t variable_name##_written_size; \
  do{ \
    IO_fd_t _IO_QuickFileWriteData_cstr_fd; \
    sint32_t _IO_QuickFileWriteData_cstr_err = IO_open((name_cstr), O_WRONLY, &_IO_QuickFileWriteData_cstr_fd); \
    if(_IO_QuickFileWriteData_cstr_err){ \
      variable_name##_written_size = (uintptr_t)_IO_QuickFileWriteData_cstr_err; \
      break; \
    } \
    variable_name##_written_size = (uintptr_t)IO_write(&_IO_QuickFileWriteData_cstr_fd, (variable_name), (buffer_size)); \
    IO_close(&_IO_QuickFileWriteData_cstr_fd); \
  }while(0); \
  if(variable_name##_written_size != (buffer_size)){ \
    __VA_ARGS__ \
  }

#define IO_QuickExistingFileWriteCSTR_cstr(path_cstr, cstr_to_write, ...) \
  { \
    const char *_IO_QuickExistingFileWriteCSTR_cstr_nocare_data = cstr_to_write; \
    IO_QuickExistingFileWriteData_cstr( \
      path_cstr, \
      _IO_QuickExistingFileWriteCSTR_cstr_nocare_data, \
      MEM_cstreu(_IO_QuickExistingFileWriteCSTR_cstr_nocare_data), \
      __VA_ARGS__) \
    ; \
  }

#define IO_QuickExistingFileWriteBase10_uint64_cstr(path_cstr, value, ...) \
  { \
    uint8_t _IO_QuickExistingFileWriteBase10_uint64_cstr_utobuf[64]; \
    uint8_t *_IO_QuickExistingFileWriteBase10_uint64_cstr_utobuf_ptr = _IO_QuickExistingFileWriteBase10_uint64_cstr_utobuf; \
    uintptr_t _IO_QuickExistingFileWriteBase10_uint64_cstr_utosize; \
    STR_uto64( \
      (value), \
      10, \
      &_IO_QuickExistingFileWriteBase10_uint64_cstr_utobuf_ptr, \
      &_IO_QuickExistingFileWriteBase10_uint64_cstr_utosize \
    ); \
    IO_QuickExistingFileWriteData_cstr( \
      path_cstr, \
      _IO_QuickExistingFileWriteBase10_uint64_cstr_utobuf_ptr, \
      _IO_QuickExistingFileWriteBase10_uint64_cstr_utosize, \
      __VA_ARGS__) \
    ; \
  }

static sint32_t _IO_LoadDefaultKernelModule_cstr(
  uint8_t *module_name,
  uintptr_t module_name_length,
  const char *param,
  uintptr_t modules_dep_size,
  uint8_t *modules_dep_ptr,
  IO_dirfd_t *fd_at
){
  uintptr_t module_flag = 0;

  uintptr_t line_begin_at = 0;
  uintptr_t full_path_length = (uintptr_t)-1;
  {
    uintptr_t last_slash_at_p1;
    for(uintptr_t i = 0; i < modules_dep_size;){
      if(modules_dep_ptr[i] == '/'){
        last_slash_at_p1 = i + 1;
      }
      else if(modules_dep_ptr[i] == '.'){
        if(MEM_ncmpn(&modules_dep_ptr[last_slash_at_p1], i - last_slash_at_p1, module_name, module_name_length) == false){
          for(; i < modules_dep_size; i++){
            if(modules_dep_ptr[i] == '\n'){
              i += 1;
              line_begin_at = i;
              break;
            }
          }
          continue;
        }

        uintptr_t last_dot_at_p1 = i + 1;
        for(; i < modules_dep_size; i++){
          if(modules_dep_ptr[i] == '.'){
            last_dot_at_p1 = i + 1;
          }
          else if(modules_dep_ptr[i] == ':'){
            full_path_length = i - line_begin_at;

            if(MEM_ncmpn(&modules_dep_ptr[last_dot_at_p1], i - last_dot_at_p1, "ko", 2) == false){
              module_flag |= MODULE_INIT_COMPRESSED_FILE;
            }

            bool got_space = false;
            do{
              i += 1;
              if(modules_dep_ptr[i] == '\n'){
                break;
              }
              else if(modules_dep_ptr[i] == ' '){
                got_space = true;
              }
              else if(got_space == true){
                got_space = false;
                do{
                  if(modules_dep_ptr[i] == '/'){
                    last_slash_at_p1 = i + 1;
                  }
                  else if(modules_dep_ptr[i] == '.'){
                    sint32_t err = _IO_LoadDefaultKernelModule_cstr(
                      &modules_dep_ptr[last_slash_at_p1],
                      i - last_slash_at_p1,
                      param,
                      modules_dep_size,
                      modules_dep_ptr,
                      fd_at
                    );
                    if(err && err != -EEXIST){
                      return err;
                    }
                    break;
                  }
                  i++;
                }while(i < modules_dep_size);
              }
            }while(i < modules_dep_size);

            break;
          }
        }

        break;
      }
      i += 1;
    }
  }

  if(full_path_length == (uintptr_t)-1){
    return __LINE__;
  }

  uint8_t path[PATH_MAX];

  if(full_path_length > sizeof(path) - 1){
    return __LINE__;
  }

  __builtin_memcpy(path, &modules_dep_ptr[line_begin_at], full_path_length);
  path[full_path_length] = 0;

  IO_fd_t fd;
  sint32_t err = IO_openat(fd_at, path, O_RDONLY, &fd);
  if(err){
    return err;
  }

  sintptr_t ret = syscall3(
    __NR_finit_module,
    _IO_fd_get_internal(&fd),
    (uintptr_t)param,
    module_flag
  );

  IO_close(&fd);

  return (sint32_t)ret;
}

static sint32_t IO_LoadDefaultKernelModule_cstr(const char *module_name_cstr, const char *param){
  const char bun0[] = "/lib/modules/";
  const char bun1[] = "/modules.dep";

  /* TOOD need some function to read first line */
  IO_QuickFileReadData_cstr("/proc/sys/kernel/osrelease", patty0, 64,
    return (sint32_t)patty0_data_size;
  );
  for(uintptr_t i = 0; i < patty0_data_size; i++){
    if(patty0_data[i] == '\n'){
      patty0_data_size = i;
      break;
    }
  }

  uint8_t path[
    + sizeof(bun0) - 1
    + sizeof(patty0_data)
    + sizeof(bun1) - 1
    + 1
  ];

  uint8_t *p = path;
  _memcpy_cstr_sumret(p, bun0);
  _memcpy_stackarr_sumret(p, patty0_data, patty0_data_size - sizeof(patty0_data));
  uint8_t *p_patty0 = p;
  _memcpy_cstr_sumret(p, bun1, +1);

  uintptr_t mmap_size;
  uint8_t *mmap_ptr;
  {
    IO_fd_t fd;
    sint32_t err = IO_open(path, O_RDONLY, &fd);
    if(err){
      return err;
    }
  
    IO_stat_t s;
    err = IO_fstat(&fd, &s);
    if(err){
      return err;
    }
  
    {
      IO_off_t io_off = IO_stat_GetSizeInBytes(&s);
      if((uint64_t)io_off > (uintptr_t)-1){
        __abort();
      }
      mmap_size = io_off;
    }
  
    mmap_ptr = (uint8_t *)IO_mmap(
      NULL,
      mmap_size,
      PROT_READ,
      MAP_PRIVATE,
      _IO_fd_get_internal(&fd),
      0
    );
    if((uintptr_t)mmap_ptr > (uintptr_t)-0x1000){
      err = (sint32_t)(uintptr_t)mmap_ptr;
    }

    IO_close(&fd);

    if(err){
      return err;
    }

    /* lets quick check modules.dep */
    do{
      if(mmap_size == 0){
        err = __LINE__;
        break;
      }
      if(mmap_ptr[mmap_size - 1] != '\n'){
        err = __LINE__;
        break;
      }
      /* should have at least one slash before dot with rest of end line */
      uintptr_t i = 0;
      bool got_slash = false;
      for(; i < mmap_size; i++){
        if(mmap_ptr[i] == '/'){
          got_slash = true;
        }
        else if(mmap_ptr[i] == '.'){
          if(got_slash == false){
            err = __LINE__;
          }
          break;
        }
        else if(mmap_ptr[i] == '\n'){
          got_slash = false;
        }
      }
      if(i == mmap_size){
        err = __LINE__;
        break;
      }
    }while(0);
    if(err){
      IO_munmap(mmap_ptr, mmap_size);
      return err;
    }
  }

  p = p_patty0;
  *p =  0;

  IO_fd_t fd;
  sint32_t err = IO_open(path, O_DIRECTORY, &fd);
  if(err == 0){
    err = _IO_LoadDefaultKernelModule_cstr(
      (uint8_t *)module_name_cstr,
      MEM_cstreu(module_name_cstr),
      param,
      mmap_size,
      mmap_ptr,
      (IO_dirfd_t *)&fd
    );

    IO_close(&fd);
  }

  IO_munmap(mmap_ptr, mmap_size);

  return err;
}

#include "../../print.h"
static sint32_t _IO_printf(const char *format, ...){
  IO_fd_t fd_stdout;
  IO_fd_set(&fd_stdout, _IO_RAWFD_STDOUT);
  va_list argv;
  va_start(argv, format);
  /* TODO need printf */
  IO_vprint(&fd_stdout, format, argv);
  va_end(argv);

  /* TODO returns 0 bytes always */
  return 0;
}

static void _IO_internal_open(){
  #if !defined(__WITCH_IO_allow_sigpipe)
    signal(SIGPIPE, SIG_IGN);
  #endif
}
static void _IO_internal_close(){}

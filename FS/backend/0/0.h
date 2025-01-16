#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(IO/IO.h)

#include _WITCH_PATH(include/syscall.h)

typedef IO_size_t FS_size_t;
typedef IO_ssize_t FS_ssize_t;
typedef IO_off_t FS_off_t;

#if !defined(__WITCH_FS_no_cooked)
  /* will be used in temp file functions */
  #include _WITCH_PATH(STR/ttcc.h)
  #include _WITCH_PATH(VEC/VEC.h)
#endif

/* TODO we are in linux backend but we check libc?? */
#if defined(__platform_libc)
  #include <sys/stat.h>
#endif

typedef struct{
  IO_dirfd_t fd;
}FS_dir_t;

static sint32_t _FS_dir_creat(const void *path){
  /* TODO not tested in any archicture */
  return syscall2(__NR_mkdir, (uintptr_t)path, 00700);
}

static sint32_t FS_dir_open(const void *path, uint32_t flag, FS_dir_t *dir){
  uintptr_t npath = MEM_cstreu(path);
  if(npath >= PATH_MAX){
    return -ENAMETOOLONG;
  }

  if(flag & O_CREAT){
    /* TODO */

    /* S_ISDIR is failed to found with nostdlib */

    /* order of this is probably wrong. */
    /* it should create stuff from end to begin. (incase this code is begin to end) */

    __abort();
    #if 0
    uint8_t tname[PATH_MAX];
    uintptr_t ipath = 0;
    for(; ipath < npath; ipath++){
      if(((uint8_t *)path)[ipath] != '/' && ipath + 1 != npath){
        continue;
      }
      ipath++;
      MEM_copy(path, tname, ipath);
      tname[ipath] = 0;
      IO_stat_t s;
      sint32_t err = IO_stat(tname, &s);
      if(err){
        if(err != -ENOENT){
          return -1;
        }
      }
      if(err == 0){
        if(S_ISDIR(s.st_mode)){
          continue;
        }
        return -ENOTDIR;
      }
      err = _FS_dir_creat(tname);
      if(err){
        return err;
      }
    }
    #endif
  }

  sint32_t err = IO_open(
    (const char *)path,
    flag | O_RDONLY | O_DIRECTORY,
    (IO_fd_t *)&dir->fd
  );
  if(err){
    return err;
  }

  return 0;
}

static void FS_dir_close(FS_dir_t *dir){
  IO_close((IO_fd_t *)&dir->fd);
}

#pragma pack(push, 1)
  typedef struct{
    uint64_t ino;
    uint64_t off;
    uint16_t reclen;
    uint8_t type;
  }_WITCH_FS_linux_dirent_t;
#pragma pack(pop)

typedef struct{
  FS_dir_t dir;
  uint8_t *buffer;
  uintptr_t buffer_size;

  IO_ssize_t read_size;
  IO_size_t offset;
}FS_dir_traverse_t;
static sint32_t FS_dir_traverse_open(
  FS_dir_t *dir,
  void *buffer,
  uintptr_t buffer_size,
  FS_dir_traverse_t *tra
){
  tra->dir = *dir;
  tra->buffer = (uint8_t *)buffer;
  tra->buffer_size = buffer_size;

  tra->read_size = 0;
  tra->offset = 0;

  return 0;
}
static void FS_dir_traverse_close(
  FS_dir_traverse_t *tra
){
  /* cupcake */
}
static sint32_t FS_dir_traverse(FS_dir_traverse_t *tra, const char **cstr){

  if(tra->offset == (IO_size_t)tra->read_size){
    gt_re:;

    tra->read_size = syscall3(__NR_getdents64, tra->dir.fd.fd, tra->buffer, tra->buffer_size);
    if(tra->read_size < 0){
      return tra->read_size;
    }
    if(tra->read_size == 0){
      return 1;
    }
    tra->offset = 0;
  }
  else{
    tra->offset += (*(_WITCH_FS_linux_dirent_t *)&tra->buffer[tra->offset]).reclen;

    if(tra->offset >= (IO_size_t)tra->read_size){
      goto gt_re;
    }
  }

  *cstr = (const char *)&((_WITCH_FS_linux_dirent_t *)&tra->buffer[tra->offset])[1];

  return 0;
}

static sint32_t FS_unlink(const void *path, uint32_t flag){
  return syscall1(__NR_unlink, (uintptr_t)path);
}

static sint32_t FS_unlinkn(const void *path, uintptr_t pathsize, uint32_t flag){
  uint8_t npath[PATH_MAX];
  if(pathsize >= (PATH_MAX - 1)){
    return -ENAMETOOLONG;
  }
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return FS_unlink(npath, flag);
}

static sint32_t FS_unlinkat(FS_dir_t *dir, const void *path, uint32_t flag){
  return syscall3(__NR_unlinkat, dir->fd.fd, (uintptr_t)path, flag);
}

static sint32_t FS_unlinkatn(FS_dir_t *dir, const void *path, uintptr_t pathsize, uint32_t flag){
  uint8_t npath[PATH_MAX];
  if(pathsize >= (PATH_MAX - 1)){
    return -ENAMETOOLONG;
  }
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return FS_unlinkat(dir, npath, flag);
}

#if !defined(__WITCH_FS_no_cooked)

  enum{
    _FS_file_FileSystem_e,
    _FS_file_Temporarily_e
  };

  typedef struct{
    uint8_t Type;
    union{
      struct{
        IO_fd_t fd;
      }FileSystem;
      struct{
        IO_off_t Offset;
        VEC_t vector;
      }Temporarily;
    };
  }FS_file_t;

  static void FS_file_getfd(FS_file_t *file, IO_fd_t *fd){
    switch(file->Type){
      case _FS_file_FileSystem_e:{
        *fd = file->FileSystem.fd;
        break;
      }
      case _FS_file_Temporarily_e:{
        PR_abort();
        break;
      }
    }
  }

  static sint32_t FS_file_opentmp(FS_file_t *file){
    file->Type = _FS_file_Temporarily_e;
    file->Temporarily.Offset = 0;
    VEC_init(&file->Temporarily.vector, 1, A_resize);
    return 0;
  }

  static bool _FS_file_rename_passpath(STR_ttcc_t *ttcc){
    return 1;
  }
  static void _FS_file_GetPathDirectory(const void *src, uintptr_t src_length, void *dst){
    src_length--;
    while(src_length != (uintptr_t)-1){
      if(((uint8_t *)src)[src_length] == '/'){
        MEM_copy(src, dst, src_length + 1);
        ((uint8_t *)dst)[src_length + 1] = 0;
        return;
      }
      src_length--;
    }
    ((uint8_t *)dst)[0] = 0;
  }
  static bool _FS_file_GetFileName(const void *src, uintptr_t src_length, void *dst){
    uintptr_t src_i = src_length;
    src_i--;
    while(src_i != (uintptr_t)-1){
      if(((uint8_t *)src)[src_i] == '/'){
        MEM_copy(&((uint8_t *)src)[src_i + 1], dst, src_length - src_i);
        ((uint8_t *)dst)[src_length - src_i] = 0;
        return 0;
      }
      src_i--;
    }
    if(src_length){
      MEM_copy(src, dst, src_length);
      ((uint8_t *)dst)[src_length] = 0;
      return 0;
    }
    return 1;
  }
  static sint32_t FS_file_rename(FS_file_t *file, const void *path){
    switch(file->Type){
      case _FS_file_FileSystem_e:{
        PR_abort();
        return 0;
      }
      case _FS_file_Temporarily_e:{
        uintptr_t path_length = MEM_cstreu(path);
        uint8_t FileName[PATH_MAX];
        if(_FS_file_GetFileName(path, path_length, FileName)){
          return -1;
        }
        uint8_t PathDirectory[PATH_MAX];
        _FS_file_GetPathDirectory(path, path_length, PathDirectory);
        uint8_t tmppath[PATH_MAX];
        STR_ttcc_t ttcc;
        ttcc.f = _FS_file_rename_passpath;
        ttcc.c = 0;
        ttcc.p = sizeof(tmppath);
        ttcc.ptr = tmppath;
        if(STR_FSttcc(&ttcc, "%s%s.tmp%c", PathDirectory, FileName, 0)){
          return -ENAMETOOLONG;
        }
        IO_fd_t fd;
        sint32_t err = IO_open(tmppath, O_WRONLY | O_CREAT, &fd);
        if(err){
          PR_abort();
        }
        if((uintptr_t)IO_write(
          &fd,
          file->Temporarily.vector.ptr,
          file->Temporarily.vector.Current
        ) != file->Temporarily.vector.Current){
          PR_abort();
          if(IO_close(&fd) != 0){
            PR_abort();
          }
          return -1;
        }
        if(IO_rename(tmppath, path) != 0){
          PR_abort();
        }
        VEC_free(&file->Temporarily.vector);
        file->Type = _FS_file_FileSystem_e;
        file->FileSystem.fd = fd;
        return 0;
      }
    }
  }

  static sint32_t FS_file_open(const void *path, FS_file_t *file, uint32_t flag){
    file->Type = _FS_file_FileSystem_e;
    return IO_open(path, flag, &file->FileSystem.fd);
  }

  static sint32_t FS_file_openn(const void *path, uintptr_t pathsize, FS_file_t *file, uint32_t flag){
    uint8_t npath[PATH_MAX];
    if(pathsize >= (PATH_MAX - 1)){
      return -ENAMETOOLONG;
    }
    MEM_copy(path, npath, pathsize);
    npath[pathsize] = 0;
    return FS_file_open(npath, file, flag);
  }

  static sint32_t FS_file_openat(FS_dir_t *dir, const void *path, FS_file_t *file, uint32_t flag){
    file->Type = _FS_file_FileSystem_e;
    return IO_openat(&dir->fd, path, flag, &file->FileSystem.fd);
  }

  static sint32_t FS_file_openatn(FS_dir_t *dir, const void *path, uintptr_t pathsize, FS_file_t *file, uint32_t flag){
    uint8_t npath[PATH_MAX];
    if(pathsize >= (PATH_MAX - 1)){
      return -ENAMETOOLONG;
    }
    MEM_copy(path, npath, pathsize);
    npath[pathsize] = 0;
    return FS_file_openat(dir, npath, file, flag);
  }

  enum{
    FS_file_seek_Begin = SEEK_SET,
    FS_file_seek_Current = SEEK_CUR,
    FS_file_seek_End = SEEK_END
  };

  static void FS_file_seek(FS_file_t *file, FS_off_t offset, uint32_t flag){
    switch(file->Type){
      case _FS_file_FileSystem_e:{
        PR_abort();
        return;
      }
      case _FS_file_Temporarily_e:{
        switch(flag){
          case FS_file_seek_Begin:{
            file->Temporarily.Offset = offset;
            return;
          }
          case FS_file_seek_Current:{
            file->Temporarily.Offset += offset;
            return;
          }
          case FS_file_seek_End:{
            file->Temporarily.Offset = file->Temporarily.vector.Current + offset;
            return;
          }
        }
        PR_abort();
      }
    }
  }

  static FS_ssize_t FS_file_read(FS_file_t *file, void *data, FS_size_t size){
    switch(file->Type){
      case _FS_file_FileSystem_e:{
        return IO_read(&file->FileSystem.fd, data, size);
      }
      case _FS_file_Temporarily_e:{
        PR_abort();
        return 0;
      }
    }
  }

  static FS_ssize_t FS_file_write(FS_file_t *file, const void *data, FS_size_t size){
    switch(file->Type){
      case _FS_file_FileSystem_e:{
        return IO_write(&file->FileSystem.fd, data, size);
      }
      case _FS_file_Temporarily_e:{
        FS_off_t of = file->Temporarily.Offset + size - file->Temporarily.vector.Current;
        if(of > 0){
          VEC_handle0(&file->Temporarily.vector, size);
        }
        MEM_copy(data, &file->Temporarily.vector.ptr[file->Temporarily.Offset], size);
        file->Temporarily.Offset += size;
        return size;
      }
    }
  }

  static sint32_t FS_file_close(FS_file_t *file){
    switch(file->Type){
      case _FS_file_FileSystem_e:{
        return IO_close(&file->FileSystem.fd);
      }
      case _FS_file_Temporarily_e:{
        PR_abort();
        return 0;
      }
    }
  }

#endif /* !defined(__WITCH_FS_no_cooked) */

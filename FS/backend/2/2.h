#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(IO/IO.h)

#include _WITCH_PATH(include/syscall.h)

typedef IO_size_t FS_size_t;
typedef IO_ssize_t FS_ssize_t;
typedef IO_off_t FS_off_t;

/* will be used in temp file functions */
#include _WITCH_PATH(STR/ttcc.h)
/* will be used to create random file name for temp file function */
/* but its not thread safe so its a TODO */
#include _WITCH_PATH(RAND/RAND.h)

#include <sys/stat.h>
/* TODO dont use this */
#include <dirent.h>

typedef struct{
  DIR *dir;
  IO_dirfd_t fd;
}FS_dir_t;

static sint32_t _FS_dir_creat(const void *path){
  return mkdir((const char *)path, S_IRWXU);
}

sint32_t FS_dir_open(const void *path, FS_dir_t *dir, int flag){
  uintptr_t npath = MEM_cstreu(path);
  if(npath >= PATH_MAX){
    return -ENAMETOOLONG;
  }

  if(flag & O_CREAT){
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
        if(err != ENOENT){
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
  }

  dir->dir = opendir(path);
  if(!dir->dir){
    return -1;
  }
  dir->fd = dirfd(dir->dir);
  return 0;
}

sint32_t FS_dir_close(FS_dir_t *dir){
  /* TODO */
  return closedir(dir->dir);
}

typedef struct{
  FS_dir_t *dir;
  const char *name;
  uint64_t size;
}FS_dir_traverse_t;
uint8_t _FS_dir_traverse(FS_dir_t *dir, FS_dir_traverse_t *arg){
  arg->dir = dir;
  return 0;
}
uint8_t FS_dir_traverse(FS_dir_traverse_t *arg){
  struct dirent *ent = readdir(arg->dir->dir);
  if(!ent){
    rewinddir(arg->dir->dir);
    return 0;
  }
  arg->name = ent->d_name;
  struct stat vstat;
  fstatat(arg->dir->fd, arg->name, &vstat, 0);
  arg->size = vstat.st_size;
  return 1;
}
#define FS_dir_traverse(dir_m) \
  for(uint8_t _mv = 1; _mv;) \
  for(FS_dir_traverse_t mv; _mv; _mv = 0) \
  if(!_FS_dir_traverse(dir_m, &mv)) \
  for(; FS_dir_traverse(&mv);)

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

static IO_fd_t FS_file_getfd(FS_file_t *file){
  switch(file->Type){
    case _FS_file_FileSystem_e:{
      return file->FileSystem.fd;
    }
    case _FS_file_Temporarily_e:{
      PR_abort();
      return 0;
    }
  }
}

sint32_t FS_unlink(const void *path, uint32_t flag){
  return -syscall1_noerr(SYS_unlink, path);
}

sint32_t FS_unlinkn(const void *path, uintptr_t pathsize, uint32_t flag){
  uint8_t npath[PATH_MAX];
  if(pathsize >= (PATH_MAX - 1)){
    return -ENAMETOOLONG;
  }
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return FS_unlink(npath, flag);
}

sint32_t FS_unlinkat(FS_dir_t *dir, const void *path, uint32_t flag){
  return -syscall3_noerr(SYS_unlinkat, dir->fd, path, flag);
}

sint32_t FS_unlinkatn(FS_dir_t *dir, const void *path, uintptr_t pathsize, uint32_t flag){
  uint8_t npath[PATH_MAX];
  if(pathsize >= (PATH_MAX - 1)){
    return -ENAMETOOLONG;
  }
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return FS_unlinkat(dir, npath, flag);
}

sint32_t FS_file_opentmp(FS_file_t *file){
  file->Type = _FS_file_Temporarily_e;
  file->Temporarily.Offset = 0;
  VEC_init(&file->Temporarily.vector, 1, A_resize);
  return 0;
}

bool _FS_file_rename_passpath(STR_ttcc_t *ttcc){
  return 1;
}
void _FS_file_GetPathDirectory(const void *src, uintptr_t src_length, void *dst){
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
bool _FS_file_GetFileName(const void *src, uintptr_t src_length, void *dst){
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
sint32_t FS_file_rename(FS_file_t *file, const void *path){
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
      IO_fd_t fd = IO_open(tmppath, O_WRONLY | O_CREAT);
      if(IO_write(
        fd,
        file->Temporarily.vector.ptr,
        file->Temporarily.vector.Current
      ) != file->Temporarily.vector.Current){
        PR_abort();
        if(IO_close(fd) != 0){
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
  file->FileSystem.fd = IO_open(path, flag);
  if(file->FileSystem.fd < 0){
    return file->FileSystem.fd;
  }
  return 0;
}

sint32_t FS_file_openn(const void *path, uintptr_t pathsize, FS_file_t *file, uint32_t flag){
  uint8_t npath[PATH_MAX];
  if(pathsize >= (PATH_MAX - 1)){
    return -ENAMETOOLONG;
  }
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return FS_file_open(npath, file, flag);
}

sint32_t FS_file_openat(FS_dir_t *dir, const void *path, FS_file_t *file, uint32_t flag){
  file->Type = _FS_file_FileSystem_e;
  file->FileSystem.fd = IO_openat(dir->fd, path, flag);
  if(file->FileSystem.fd < 0){
    return file->FileSystem.fd;
  }
  return 0;
}

sint32_t FS_file_openatn(FS_dir_t *dir, const void *path, uintptr_t pathsize, FS_file_t *file, uint32_t flag){
  uint8_t npath[PATH_MAX];
  if(pathsize >= (PATH_MAX - 1)){
    return -ENAMETOOLONG;
  }
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return FS_file_openat(dir, npath, file, flag);
}

static FS_ssize_t FS_file_read(FS_file_t *file, void *data, FS_size_t size){
  switch(file->Type){
    case _FS_file_FileSystem_e:{
      return IO_read(file->FileSystem.fd, data, size);
    }
    case _FS_file_Temporarily_e:{
      PR_abort();
      return 0;
    }
  }
}

FS_ssize_t FS_file_write(FS_file_t *file, const void *data, FS_size_t size){
  switch(file->Type){
    case _FS_file_FileSystem_e:{
      return IO_write(file->FileSystem.fd, data, size);
    }
    case _FS_file_Temporarily_e:{
      if(file->Temporarily.Offset != file->Temporarily.vector.Current){
        PR_abort();
      }
      VEC_handle0(&file->Temporarily.vector, size);
      MEM_copy(data, &file->Temporarily.vector.ptr[file->Temporarily.Offset], size);
      file->Temporarily.Offset += size;
      return size;
    }
  }
}

static sint32_t FS_file_close(FS_file_t *file){
  switch(file->Type){
    case _FS_file_FileSystem_e:{
      return IO_close(file->FileSystem.fd);
    }
    case _FS_file_Temporarily_e:{
      PR_abort();
      return 0;
    }
  }
}

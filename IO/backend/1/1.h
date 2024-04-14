#ifndef IO_set_fd_limit
  /* i read somewhere windows only can open 0x200 fd */
  /* but i also read windows can open 0xffff sockets */
  /* since socket is fd lets put socket limit */
  /* also user; dont put that limit too low. */
  /* because windows doesnt start fd count from 0 */
  #define IO_set_fd_limit 0xffff
#endif

#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(PR/PR.h)
#include _WITCH_PATH(A/A.h)

#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <BaseTsd.h>
#include <io.h>
#include <winerror.h>
#include _WITCH_PATH(include/windows/windows.h)
#include _WITCH_PATH(include/windows/winsock.h)

#define O_RDONLY _O_RDONLY
#define O_WRONLY _O_WRONLY
#define O_RDWR _O_RDWR
#ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
#endif

#ifndef STDIN_FILENO
  #define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
  #define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
  #define STDERR_FILENO 2
#endif

typedef sint64_t IO_off_t;
typedef SIZE_T IO_size_t;
typedef SSIZE_T IO_ssize_t;

typedef struct{
  sint32_t fd;
}IO_fd_t;
typedef struct{
  HANDLE handle;
}IO_dirfd_t;

static void IO_fd_set(IO_fd_t *fd, sint32_t description){
  fd->fd = description;
}

static sint32_t IO_getpath(const IO_dirfd_t *dirfd, uint8_t *path){
  if(GetFinalPathNameByHandleA(dirfd->handle, (LPSTR)path, PATH_MAX, 0) < 0){
    return -1;
  }
  return 0;
}

typedef struct __stat64 IO_stat_t;

static sint32_t IO_stat(const void *path, IO_stat_t *s){
  if(_stat64((const char *)path, s) < 0){
    return -errno;
  }
  return 0;
}
static sint32_t IO_fstat(const IO_fd_t *fd, IO_stat_t *s){
  return _fstat64(fd->fd, s);
}

static IO_off_t IO_stat_GetSizeInBytes(IO_stat_t *s){
  return s->st_size;
}

enum{
  _IO_fd_unknown_e,
  _IO_fd_tty_e,
  _IO_fd_file_e,
  _IO_fd_socket_e
};
static uint8_t *_IO_fd_nodes = NULL;
static void _IO_assign_fd(const IO_fd_t *fd, uint8_t type){
  if(fd->fd >= IO_set_fd_limit){
    PR_abort();
  }
  if(type == _IO_fd_unknown_e){
    if(_IO_fd_nodes[fd->fd] == _IO_fd_unknown_e){
      PR_abort();
    }
  }
  else{
    if(_IO_fd_nodes[fd->fd] != _IO_fd_unknown_e){
      PR_abort();
    }
  }
  _IO_fd_nodes[fd->fd] = type;
}
static uint8_t _IO_get_fd(const IO_fd_t *fd){
  if(fd->fd >= IO_set_fd_limit){
    PR_abort();
  }
  if(_IO_fd_nodes[fd->fd] == _IO_fd_unknown_e){
    PR_abort();
  }
  return _IO_fd_nodes[fd->fd];
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

static bool IO_pipe(IO_fd_t *fds, IO_pipe_Flag Flag){
  union{
    struct sockaddr_in inaddr;
    struct sockaddr addr;
  }usockaddr = {0};
  usockaddr.inaddr.sin_family = AF_INET;
  usockaddr.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  usockaddr.inaddr.sin_port = 0;

  socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);

  fds[0].fd = INVALID_SOCKET;
  fds[1].fd = INVALID_SOCKET;
  if(Flag & IO_pipe_Flag_Packet){
    do{
      DWORD InternalFlag;

      InternalFlag = 0;
      if(Flag & IO_pipe_Flag_NonblockRead){
        InternalFlag = WSA_FLAG_OVERLAPPED;
      }

      fds[0].fd = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, InternalFlag);
      if(fds[0].fd == INVALID_SOCKET){
        break;
      }
      if(bind(fds[0].fd, &usockaddr.addr, sizeof(usockaddr.inaddr)) == SOCKET_ERROR){
        break;
      }
      if(getsockname(fds[0].fd, &usockaddr.addr, &sockaddr_in_size) == SOCKET_ERROR){
        break;
      }

      InternalFlag = 0;
      if(Flag & IO_pipe_Flag_NonblockWrite){
        InternalFlag = WSA_FLAG_OVERLAPPED;
      }
      fds[1].fd = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, InternalFlag);
      if(fds[1].fd == INVALID_SOCKET){
        break;
      }
      if(connect(fds[1].fd, &usockaddr.addr, sizeof(usockaddr.inaddr)) == SOCKET_ERROR){
        break;
      }

      _IO_assign_fd(&fds[0], _IO_fd_socket_e);
      _IO_assign_fd(&fds[1], _IO_fd_socket_e);

      return 0;
    }while(0);

    closesocket(fds[0].fd);
    closesocket(fds[1].fd);

    return 1;
  }
  else{
    int reuse = 1;

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listener == INVALID_SOCKET){
      return SOCKET_ERROR;
    }

    do{
      if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, (socklen_t)sizeof(reuse)) == -1){
        break;
      }
      if(bind(listener, &usockaddr.addr, sizeof(usockaddr.inaddr)) == SOCKET_ERROR){
        break;
      }
      if(getsockname(listener, &usockaddr.addr, &sockaddr_in_size) == SOCKET_ERROR){
        break;
      }
      if(listen(listener, 1) == SOCKET_ERROR){
        break;
      }
      DWORD InternalFlag = 0;
      if(Flag & IO_pipe_Flag_NonblockRead){
        InternalFlag = WSA_FLAG_OVERLAPPED;
      }
      fds[0].fd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, InternalFlag);
      if(fds[0].fd == INVALID_SOCKET){
        break;
      }
      if(connect(fds[0].fd, &usockaddr.addr, sizeof(usockaddr.inaddr)) == SOCKET_ERROR){
        break;
      }
      fds[1].fd = accept(listener, NULL, NULL);
      if(fds[1].fd == INVALID_SOCKET){
        break;
      }
      _IO_assign_fd(&fds[0], _IO_fd_socket_e);
      _IO_assign_fd(&fds[1], _IO_fd_socket_e);

      closesocket(listener);

      return 0;
    }while(0);

    closesocket(listener);
    closesocket(fds[0].fd);
    closesocket(fds[1].fd);
    return 1;
  }
}

static sint32_t IO_open(const void *path, uint32_t flag, IO_fd_t *fd){
  sint32_t description = _open((const char *)path, flag | _O_BINARY, _S_IREAD | _S_IWRITE);
  if(description >= 0){
    fd->fd = description;
    IO_fd_set(fd, description);
    _IO_assign_fd(fd, _IO_fd_file_e);
    return 0;
  }
  return -errno;
}

static sint32_t IO_openat(const IO_dirfd_t *dirfd, const void *path, uint32_t flag, IO_fd_t *fd){
  uint8_t ppath[PATH_MAX + 1];
  if(IO_getpath(dirfd, ppath)){
    return -1;
  }
  uint8_t spath[PATH_MAX + 1];
  sprintf((char *const)spath, "%s\\%s", (const char *const)ppath, path);
  return IO_open(spath, flag, fd);
}

static sint32_t IO_openatn(const IO_dirfd_t *dirfd, const void *ppath, uintptr_t pathsize, uint32_t flag, IO_fd_t *fd){
  const char *path = (const char *)ppath;
  char npath[PATH_MAX];
  MEM_copy(path, npath, pathsize);
  npath[pathsize] = 0;
  return IO_openat(dirfd, npath, flag, fd);
}

static sint32_t IO_close(const IO_fd_t *fd){
  sint32_t err = _close(fd->fd);
  if(err >= 0){
    _IO_assign_fd(fd, _IO_fd_unknown_e);
  }
  return err;
}

static sint32_t IO_truncate(const IO_fd_t *fd, IO_off_t size){
  HANDLE h = (HANDLE)_get_osfhandle(fd->fd);
  if(SetFileValidData(h, size)){
    return 0;
  }
  else{
    return -1;
  }
}

static IO_off_t IO_lseek(const IO_fd_t *fd, IO_off_t isize, int state){
  return _lseeki64(fd->fd, isize, state);
}

static IO_off_t IO_ltell(const IO_fd_t *fd){
  return _telli64(fd->fd);
}

static IO_ssize_t IO_read(const IO_fd_t *fd, void *data, IO_size_t size){
  static uint32_t _IO_stdin_size = 0;
  switch(_IO_get_fd(fd)){
    case _IO_fd_tty_e:{
      INPUT_RECORD record;
      DWORD numRead;
      if(!PeekConsoleInput((HANDLE)_get_osfhandle(fd->fd), &record, 1, &numRead)){
        PR_abort();
      }
      if (numRead < 1) {//keep carefully
        return 0;
      }
      if(record.EventType != KEY_EVENT){
        if(!ReadConsoleInput((HANDLE)_get_osfhandle(fd->fd), &record, 1, &numRead)){
          PR_abort();
        }
        return 0;
      }
      if(!record.Event.KeyEvent.bKeyDown){
        if(!ReadConsoleInput((HANDLE)_get_osfhandle(fd->fd), &record, 1, &numRead)){
          PR_abort();
        }
        return 0;
      }
      int end = WideCharToMultiByte(CP_UTF8, 0, &record.Event.KeyEvent.uChar.UnicodeChar, 1, 0, 0,  NULL, NULL);
      if(end > size){
        return 0;
      }
      if(!ReadConsoleInput((HANDLE)_get_osfhandle(fd->fd), &record, 1, &numRead)){
        PR_abort();
      }
      if(record.EventType != KEY_EVENT){
        return 0;
      }
      if(!record.Event.KeyEvent.bKeyDown){
        return 0;
      }
      end = WideCharToMultiByte(CP_UTF8, 0, &record.Event.KeyEvent.uChar.UnicodeChar, 1, 0, 0,  NULL, NULL);
      if(end > size){
        /* we lost a character here */
        return 0;
      }
      if(!record.Event.KeyEvent.uChar.UnicodeChar){
        return 0;
      }
      WideCharToMultiByte(CP_UTF8, 0, &record.Event.KeyEvent.uChar.UnicodeChar, 1, (LPSTR)data, size,  NULL, NULL);
      // backspace
      if(record.Event.KeyEvent.uChar.UnicodeChar == 0x08){
        if (_IO_stdin_size == 0) {
          return 0;
        }
        --_IO_stdin_size;

        uint8_t stdout_buffer[3] = {0x08, ' ', 0x08};

        int r = _write(1, stdout_buffer, 3);
        if(r == -1){
          PR_abort();
        }
      }
      else{
        ++_IO_stdin_size;
        // enter
        if (strchr((char*)data, '\r') != 0) {
          _IO_stdin_size = 0;
        }
        int r = _write(1, data, end);
        if(r == -1){
          PR_abort();
        }
      }
      return end;
    }
    case _IO_fd_file_e:{
      int r = _read(fd->fd, data, size);
      if(r == -1){
        if(errno == EAGAIN){
          return 0;
        }
        else{
          return -errno;
        }
      }
      return r;
    }
    case _IO_fd_socket_e:{
      int r = recv(fd->fd, (char *)data, (int)size, 0);
      if(r == SOCKET_ERROR){
        int err = WSAGetLastError();
        switch(err){
          case WSAEWOULDBLOCK:{
            return 0;
          }
          default:{
            return -err;
          }
        }
      }
      if(r == 0){
        return -WSAECONNRESET;
      }
      return r;
    }
  }
}

static IO_ssize_t IO_pread(const IO_fd_t *fd, void *data, IO_off_t isize, IO_size_t nsize){
  IO_off_t current = IO_ltell(fd);
  if(current < 0){
    return -1;
  }
  if(IO_lseek(fd, isize, SEEK_CUR) < 0){
    return -1;
  }
  IO_ssize_t len = IO_read(fd, data, nsize);
  if(len < 0){
    return -1;
  }
  int read_errno = errno;
  if(IO_lseek(fd, current, SEEK_SET) < 0){
    /* stupid problems */
    PR_abort();
  }
  if(len < 0){
    if(read_errno == EAGAIN){
      return 0;
    }
    else{
      return -1;
    }
  }
  return len;
}

static IO_ssize_t IO_write(const IO_fd_t *fd, const void *data, IO_size_t size){
  switch(_IO_get_fd(fd)){
    case _IO_fd_tty_e:
    case _IO_fd_file_e:{
      int r = _write(fd->fd, data, size);
      if(r == -1){
        if(errno == EAGAIN){
          return 0;
        }
        else{
          return -1;
        }
      }
      return r;
    }
    case _IO_fd_socket_e:{
      int r = send(fd->fd, (const char *)data, (int)size, 0);
      if(r == SOCKET_ERROR){
        int err = WSAGetLastError();
        switch(err){
          case WSAEWOULDBLOCK:{
            return 0;
          }
          default:{
            return -1;
          }
        }
      }
      return r;
    }
  }
}

static IO_off_t IO_sendfile(const IO_fd_t *in, const IO_fd_t *out, IO_off_t isize, IO_off_t nsize){
  uint8_t data[PAGE_SIZE];
  IO_size_t datalen = nsize < sizeof(data) ? nsize : sizeof(data);
  IO_ssize_t readlen = IO_pread(in, data, isize, datalen);
  if(readlen < 0){
    return -1;
  }
  IO_ssize_t writelen = IO_write(out, data, readlen);
  if(writelen < 0){
    return -1;
  }
  return writelen;
}

static sint32_t IO_rename(const void *src, const void *dst){
  if(MoveFileExA((LPCSTR)src, (LPCSTR)dst, MOVEFILE_REPLACE_EXISTING)){
    return 0;
  }
  return -1;
}

static sint32_t IO_access(const void *path){
  PR_abort();
  return -1;
}

static bool IO_IsPathExists(const void *path){
  return GetFileAttributes((LPCSTR)path) != INVALID_FILE_ATTRIBUTES;
}

static void _IO_internal_open(){
  _IO_fd_nodes = A_resize(NULL, IO_set_fd_limit);
  MEM_set(_IO_fd_unknown_e, _IO_fd_nodes, IO_set_fd_limit);
  IO_fd_t fd;
  IO_fd_set(&fd, STDIN_FILENO);
  _IO_assign_fd(&fd, _IO_fd_tty_e);
  IO_fd_set(&fd, STDOUT_FILENO);
  _IO_assign_fd(&fd, _IO_fd_tty_e);
  IO_fd_set(&fd, STDERR_FILENO);
  _IO_assign_fd(&fd, _IO_fd_tty_e);
}
static void _IO_internal_close(){
  A_resize(_IO_fd_nodes, NULL);
}

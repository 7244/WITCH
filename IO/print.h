#pragma once

#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(STR/ttcc.h)

static bool _IO_vprint_buf(STR_ttcc_t *ttcc){
  IO_write((IO_fd_t *)ttcc->arg, ttcc->ptr, ttcc->c);
  ttcc->c = 0;
  return 0;
}
static bool IO_vprint(const IO_fd_t *outfd, const char *format, va_list argv){
  uint8_t buf[4096];
  STR_ttcc_t ttcc;
  ttcc.ptr = buf;
  ttcc.c = 0;
  ttcc.p = sizeof(buf);
  ttcc.f = _IO_vprint_buf;
  ttcc.arg = (void *)outfd;
  if(STR_vFSttcc(&ttcc, format, argv)){
    return 1;
  }
  if(ttcc.c){
    return _IO_vprint_buf(&ttcc);
  }
  return 0;
}
static bool IO_print(const IO_fd_t *outfd, const char *format, ...){
  va_list argv;
  va_start(argv, format);
  bool r = IO_vprint(outfd, format, argv);
  va_end(argv);
  return r;
}

#pragma once

#include _WITCH_PATH(IO/print.h)

static int printf(const char *format, ...){
  IO_fd_t fd_stdout;
  IO_fd_set(&fd_stdout, FD_OUT);
  va_list argv;
  va_start(argv, format);
  IO_vprint(&fd_stdout, format, argv);
  va_end(argv);

  /* TOOD dont return 0 */
  return 0;
}

static int puts(const char *str){
  return printf("%s\n", str);
}

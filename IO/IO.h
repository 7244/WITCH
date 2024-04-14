#ifndef _WITCH_libdefine_IO
#define _WITCH_libdefine_IO

#ifndef IO_set_backend
  #if defined(WOS_UNIX_LINUX)
    #define IO_set_backend 0
  #elif defined(WOS_WINDOWS)
    #define IO_set_backend 1
  #elif defined(WOS_UNIX_BSD)
    #define IO_set_backend 2
  #else
    #error IO_set_backend is not defined
  #endif
#endif

typedef enum{
  IO_pipe_Flag_NoFlag = 0x00,
  IO_pipe_Flag_Packet = 0x01,
  IO_pipe_Flag_NonblockRead = 0x02,
  IO_pipe_Flag_NonblockWrite = 0x04
}IO_pipe_Flag;

#if defined(WL_CPP)
  inline IO_pipe_Flag operator|(IO_pipe_Flag a, IO_pipe_Flag b){
    return static_cast<IO_pipe_Flag>((uint32_t)a | (uint32_t)b);
  }
#endif

#if IO_set_backend == 0
  #include "backend/0/0.h"
#elif IO_set_backend == 1
  #include "backend/1/1.h"
#elif IO_set_backend == 2
  #include "backend/2/2.h"
#else
  #error ?
#endif

#ifdef _WITCH_libdefine_PlatformOpen
  #error ?
#endif

#ifdef PRE
  PRE{
    _IO_internal_open();
  }
#endif
/* TODO _IO_internal_close needs to be called if its automaticly possible */

#endif

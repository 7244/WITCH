#ifndef _WITCH_libdefine_include_windows_winsock
#define _WITCH_libdefine_include_windows_winsock

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#define SOCK_NONBLOCK 00004000
typedef int socklen_t;
#pragma comment(lib, "ws2_32.lib")

static void _WITCH_include_windows_winsock_internal_open(){
  WSADATA wsa;
  if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
    __abort();
  }
}
static void _WITCH_include_windows_winsock_internal_close(){
  
}

#ifdef _WITCH_libdefine_PlatformOpen
  #error ?
#endif

#ifdef PRE
  PRE{
    _WITCH_include_windows_winsock_internal_open();
  }
#endif
/* TODO _WITCH_include_windows_winsock_internal_close needs to be called if its automaticly possible */

#endif

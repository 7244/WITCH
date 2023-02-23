#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#define SOCK_NONBLOCK 00004000
typedef int socklen_t;
#pragma comment(lib, "ws2_32.lib")
PRE{
  WSADATA wsa;
  if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0){
    PR_abort();
  }
}

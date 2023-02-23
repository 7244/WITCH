#pragma once

#include _WITCH_PATH(MD/Mice.h)
#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(EV/EV.h)

#if MD_Mice_set_backend == 0
  #include <X11/Xlib.h>
#elif MD_Mice_set_backend == 1
  #include <xcb/xcb.h>
  #include <xcb/xtest.h>
#elif MD_Mice_set_backend == 2
  #include _WITCH_PATH(include/windows/windows.h)
  #include _WITCH_PATH(TH/TH.h)
  #define BLL_set_prefix _MD_Mice_ListenerList
  #define BLL_set_type_node uint32_t
  #define BLL_set_NodeData \
    IO_fd_t PipeWrite;
  #include _WITCH_PATH(BLL/BLL.h)
  _MD_Mice_ListenerList_t _MD_Mice_ListenerList;
  TH_mutex_t _MD_Mice_mutex;
  PRE{
    _MD_Mice_ListenerList_open(&_MD_Mice_ListenerList);
    TH_mutex_init(&_MD_Mice_mutex);
  }
  LRESULT CALLBACK _MD_Mice_MouseCallback(int nCode, WPARAM wParam, LPARAM lParam){
    switch(wParam){
      case WM_MOUSEMOVE:{
        MSLLHOOKSTRUCT *event = (MSLLHOOKSTRUCT *)lParam;
        uint32_t data[2];
        data[0] = event->pt.x;
        data[1] = event->pt.y;
        TH_lock(&_MD_Mice_mutex);
        _MD_Mice_ListenerList_NodeReference_t NodeReference = _MD_Mice_ListenerList_GetNodeFirst(&_MD_Mice_ListenerList);
        while(NodeReference != _MD_Mice_ListenerList.dst){
          _MD_Mice_ListenerList_Node_t *Node = _MD_Mice_ListenerList_GetNodeByReference(&_MD_Mice_ListenerList, NodeReference);
          if(IO_write(Node->data.PipeWrite, data, sizeof(data)) != sizeof(data)){
            PR_abort();
          }
          NodeReference = Node->NextNodeReference;
        }
        TH_unlock(&_MD_Mice_mutex);
        break;
      }
    }
    return CallNextHookEx(0, nCode, wParam, lParam);
  }
  HHOOK _MD_Mice_hook;
  void _MD_Mice_EV_hook(void){
    _MD_Mice_hook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)_MD_Mice_MouseCallback, NULL, 0);
    if(_MD_Mice_hook == NULL){
      PR_abort();
    }
  }
#else
  #error ?
#endif

typedef struct{
  MD_Mice_t Mice;
  IO_fd_t fd;
}MD_MiceListener_t;

bool MD_MiceListener_open(MD_MiceListener_t *MiceListener, EV_t *listener){
  #if MD_Mice_set_backend == 0
    if(MD_Mice_open(&MiceListener->Mice)){
      return 1;
    }
    MiceListener->fd = IO_open("/dev/input/mice", O_RDONLY | O_NONBLOCK);
    if(MiceListener->fd < 0){
      MD_Mice_close(&MiceListener->Mice);
      return 1;
    }
    return 0;
  #elif MD_Mice_set_backend == 1
    if(MD_Mice_open(&MiceListener->Mice)){
      return 1;
    }
    MiceListener->fd = IO_open("/dev/input/mice", O_RDONLY | O_NONBLOCK);
    if(MiceListener->fd < 0){
      MD_Mice_close(&MiceListener->Mice);
      return 1;
    }
    return 0;
  #elif MD_Mice_set_backend == 2
    uint32_t usage = _MD_Mice_ListenerList_usage(&_MD_Mice_ListenerList);
    if(usage == 0){
      EV_ListenObjects_AddFunction(listener, _MD_Mice_EV_hook);
    }
    IO_fd_t pipes[2];
    if(IO_pipe(pipes, 0) != 0){
      if(usage == 0){
        if(UnhookWindowsHookEx(_MD_Mice_hook) == 0){
          PR_abort();
        }
      }
      return 1;
    }
    MiceListener->fd = pipes[0];
    TH_lock(&_MD_Mice_mutex);
    _MD_Mice_ListenerList_NodeReference_t NodeReference = _MD_Mice_ListenerList_NewNodeLast(&_MD_Mice_ListenerList);
    _MD_Mice_ListenerList_Node_t *Node = _MD_Mice_ListenerList_GetNodeByReference(&_MD_Mice_ListenerList, NodeReference);
    Node->data.PipeWrite = pipes[1];
    TH_unlock(&_MD_Mice_mutex);
    return 0;
  #endif
}
void MD_MiceListener_close(MD_MiceListener_t *MiceListener){
  #if MD_Mice_set_backend == 0
    MD_Mice_close(&MiceListener->Mice);
    if(IO_close(MiceListener->fd) != 0){
      PR_abort();
    }
  #elif MD_Mice_set_backend == 1
    MD_Mice_close(&MiceListener->Mice);
    if(IO_close(MiceListener->fd) != 0){
      PR_abort();
    }
  #elif MD_Mice_set_backend == 2
  #endif
}

bool MD_MiceListener_Event_read(MD_MiceListener_t *MiceListener, uint32_t *x, uint32_t *y){
  #if MD_Mice_set_backend == 0
    uint8_t a[3];
    if(IO_read(MiceListener->fd, a, sizeof(a)) != sizeof(a)){
      return 1;
    }

    Window wrr, wcr;
    int f0, f1;
    unsigned int f2;
    XQueryPointer(MiceListener->Mice.display, MiceListener->Mice.window, &wrr, &wcr, (int *)x, (int *)y, &f0, &f1, &f2);
    return 0;
  #elif MD_Mice_set_backend == 1
    uint8_t a[3];
    if(IO_read(MiceListener->fd, a, sizeof(a)) != sizeof(a)){
      return 1;
    }

    xcb_query_pointer_cookie_t cookie = xcb_query_pointer(MiceListener->Mice.Connection, MiceListener->Mice.Window);
    xcb_query_pointer_reply_t *reply = xcb_query_pointer_reply(MiceListener->Mice.Connection, cookie, NULL);
    if(reply == NULL){
      PR_abort();
    }
    *x = reply->root_x;
    *y = reply->root_y;
    return 0;
  #elif MD_Mice_set_backend == 2
    uint32_t data[2];
    if(IO_read(MiceListener->fd, data, sizeof(data)) != sizeof(data)){
      PR_abort();
    }
    *x = data[0];
    *y = data[1];
    return 0;
  #endif
}

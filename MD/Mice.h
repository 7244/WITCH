#pragma once

#include _WITCH_PATH(MD/MD.h)

#ifndef MD_Mice_set_backend
  #if MD_set_backend == 0
    #if MD_API_set_backend == 0
      #define MD_Mice_set_backend 0
    #elif MD_API_set_backend == 1
      #define MD_Mice_set_backend 1
    #else
      #error ?
    #endif
  #elif MD_set_backend == 1
    #if MD_API_set_backend == 0
      #define MD_Mice_set_backend 2
    #else
      #error ?
    #endif
  #else
    #error ?
  #endif
#endif

#if MD_Mice_set_backend == 0
  #include <X11/Xlib.h>

  typedef struct{
    Display *display;
    Window window;
  }MD_Mice_t;
#elif MD_Mice_set_backend == 1
  #include <xcb/xcb.h>
  #include <xcb/xtest.h>

  typedef struct{
    xcb_connection_t *Connection;
    xcb_window_t Window;
  }MD_Mice_t;
#elif MD_Mice_set_backend == 2
  #include _WITCH_PATH(include/windows/windows.h)

  typedef struct{
    uint8_t filler;
  }MD_Mice_t;
#else
  #error ?
#endif

typedef enum{
  MD_Mice_Error_Success,
  MD_Mice_Error_Temporary,
  MD_Mice_Error_UnknownArgument,
  MD_Mice_Error_InternalError
}MD_Mice_Error;

MD_Mice_Error MD_Mice_Open(MD_Mice_t *Mice){
  #if MD_Mice_set_backend == 0
    Mice->display = XOpenDisplay(NULL);
    if(!Mice->display){
      return MD_Mice_Error_InternalError;
    }
    Mice->window = XRootWindow(Mice->display, 0);
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 1
    Mice->Connection = xcb_connect(NULL, NULL);
    int err = xcb_connection_has_error(Mice->Connection);
    if(err){
      xcb_disconnect(Mice->Connection);
      return MD_Mice_Error_InternalError;
    }

    const xcb_setup_t *setup = xcb_get_setup(Mice->Connection);

    xcb_screen_t *Screen = _MD_GetScreenByIndex(setup, 0);
    if(!Screen){
      PR_abort();
      return MD_Mice_Error_InternalError;
    }

    Mice->Window = Screen->root;
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 2
    return MD_Mice_Error_Success;
  #endif
}
void MD_Mice_Close(MD_Mice_t *Mice){
  #if MD_Mice_set_backend == 0
    XCloseDisplay(Mice->display);
  #elif MD_Mice_set_backend == 1
    xcb_disconnect(Mice->Connection);
  #elif MD_Mice_set_backend == 2
  #endif
}

MD_Mice_Error MD_Mice_Coordinate_Read(MD_Mice_t *Mice, uint32_t *x, uint32_t *y){
  #if MD_Mice_set_backend == 0
    Window wrr, wcr;
    int f0, f1;
    unsigned int f2;
    XQueryPointer(Mice->display, Mice->window, &wrr, &wcr, (int *)x, (int *)y, &f0, &f1, &f2);
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 1
    xcb_query_pointer_cookie_t cookie = xcb_query_pointer(Mice->Connection, Mice->Window);
    xcb_query_pointer_reply_t *reply = xcb_query_pointer_reply(Mice->Connection, cookie, NULL);
    if(reply == NULL){
      PR_abort();
    }
    *x = reply->root_x;
    *y = reply->root_y;
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 2
    POINT point;
    if(GetCursorPos(&point) == 0){
      return MD_Mice_Error_Temporary;
    }
    *x = point.x;
    *y = point.y;
    return MD_Mice_Error_Success;
  #endif
}
MD_Mice_Error MD_Mice_Button_Write(MD_Mice_t *Mice, uint8_t key, bool state){
  #if MD_Mice_set_backend == 0
    #error ?
  #elif MD_Mice_set_backend == 1
    uint8_t detail;
    switch(key){
      case 0:{
        detail = XCB_BUTTON_INDEX_1;
        break;
      }
      case 1:{
        detail = XCB_BUTTON_INDEX_2;
        break;
      }
      case 2:{
        detail = XCB_BUTTON_INDEX_3;
        break;
      }
      case 3:{
        detail = XCB_BUTTON_INDEX_4;
        break;
      }
      case 4:{
        detail = XCB_BUTTON_INDEX_5;
        break;
      }
      default:{
        return MD_Mice_Error_UnknownArgument;
      }
    }
    xcb_window_t window = {XCB_NONE};
    if(key >= 3 && key <= 4){
      xcb_test_fake_input(Mice->Connection, XCB_BUTTON_PRESS, detail, XCB_CURRENT_TIME, window, 0, 0, 0);
      xcb_test_fake_input(Mice->Connection, XCB_BUTTON_RELEASE, detail, XCB_CURRENT_TIME, window, 0, 0, 0);
    }
    else{
      uint8_t type = state ? XCB_BUTTON_PRESS : XCB_BUTTON_RELEASE;
      xcb_test_fake_input(Mice->Connection, type, detail, XCB_CURRENT_TIME, window, 0, 0, 0);
    }
    if(xcb_flush(Mice->Connection) <= 0){
      return MD_Mice_Error_InternalError;
    }
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 2
    sint32_t ScrollMultipler;
    DWORD InternalKey;
    switch(key){
      case 0:{
        InternalKey = state ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        break;
      }
      case 1:{
        InternalKey = state ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
        break;
      }
      case 2:{
        InternalKey = state ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
        break;
      }
      case 3:
        ScrollMultipler = +1;
        if(0){
      case 4:
          ScrollMultipler = -1;
        }
        InternalKey = MOUSEEVENTF_WHEEL;
        break;
      default:{
        return MD_Mice_Error_UnknownArgument;
      }
    }

    INPUT input;

    input.type = INPUT_MOUSE;

    input.mi.dx = 0;
    input.mi.dy = 0;
    if(InternalKey == MOUSEEVENTF_WHEEL){
      input.mi.mouseData = 120 * ScrollMultipler;
    }
    else{
      input.mi.mouseData = 0;
    }
    input.mi.dwFlags = InternalKey;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    if(SendInput(1, &input, sizeof(INPUT)) != 1){
      return MD_Mice_Error_Temporary;
    }
    return MD_Mice_Error_Success;
  #endif
}
MD_Mice_Error MD_Mice_Coordinate_Write(MD_Mice_t *Mice, uint32_t x, uint32_t y){
  #if MD_Mice_set_backend == 0
    XWarpPointer(Mice->display, Mice->window, Mice->window, 0, 0, 0, 0, x, y);
    XFlush(Mice->display);
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 1
    xcb_warp_pointer(Mice->Connection, Mice->Window, Mice->Window, 0, 0, 0, 0, x, y);
    xcb_flush(Mice->Connection);
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 2
    if(SetCursorPos((int)x, (int)y) == 0){
      return MD_Mice_Error_Temporary;
    }
    return MD_Mice_Error_Success;
  #endif
}
MD_Mice_Error MD_Mice_Motion_Write(MD_Mice_t *Mice, sint32_t x, sint32_t y){
  #if MD_Mice_set_backend == 0
    #error ?
  #elif MD_Mice_set_backend == 1
    xcb_window_t window = {XCB_NONE};
    xcb_test_fake_input(Mice->Connection, XCB_MOTION_NOTIFY, 1, 0, window, x, y, 0);
    xcb_flush(Mice->Connection);
    return MD_Mice_Error_Success;
  #elif MD_Mice_set_backend == 2
    INPUT input;

    input.type = INPUT_MOUSE;

    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    if(SendInput(1, &input, sizeof(INPUT)) != 1){
      return MD_Mice_Error_Temporary;
    }
    return MD_Mice_Error_Success;
  #endif
}

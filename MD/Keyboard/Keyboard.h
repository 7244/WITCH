#pragma once

#include _WITCH_PATH(MD/MD.h)

#ifndef MD_Keyboard_set_backend
  #if MD_set_backend == 0
    #if MD_API_set_backend == 0
      #define MD_Keyboard_set_backend 0
    #elif MD_API_set_backend == 1
      #define MD_Keyboard_set_backend 0
    #else
      #error ?
    #endif
  #elif MD_set_backend == 1
    #if MD_API_set_backend == 0
      #define MD_Keyboard_set_backend 2
    #else
      #error ?
    #endif
  #else
    #error ?
  #endif
#endif

#if MD_Keyboard_set_backend == 0
  #include <X11/Xlib.h>
  #include <X11/XKBlib.h>
  #include <X11/extensions/XTest.h>
  #include <xorg/xkbstr.h>
  #include _WITCH_PATH(STR/common/common.h)

  typedef struct{
    Display *Display;
    uint8_t ScancodeToKeycode[0x200];
  }MD_Keyboard_t;
#elif MD_Keyboard_set_backend == 1
  #include <xcb/xcb.h>
  #include <xcb/xtest.h>

  typedef struct{
    xcb_connection_t *Connection;
  }MD_Keyboard_t;
#elif MD_Keyboard_set_backend == 2
  typedef struct{

  }MD_Keyboard_t;
#else
  #error ?
#endif

typedef enum{
  MD_Keyboard_Error_Success,
  MD_Keyboard_Error_Temporary,
  MD_Keyboard_Error_UnknownArgument,
  MD_Keyboard_Error_InternalError
}MD_Keyboard_Error;

#if MD_Keyboard_set_backend == 0
  typedef struct{
    uint8_t Name[XkbKeyNameLength];
    uint16_t Scancode;
  }_MD_Keyboard_ScancodeNameAndScancode_t;
  uint16_t _MD_Keyboard_ScancodeFromXkbDesc(XkbDescPtr desc, uint8_t Keycode){
    _MD_Keyboard_ScancodeNameAndScancode_t Table[] = {
      #include "internal/x11_ScancodenameList.h"
    };
    #if XkbKeyNameLength != 4
      #error enjoy pain
    #endif
    uint32_t Name = *(uint32_t *)desc->names->keys[Keycode].name;
    uint16_t i = 0;
    for(; i < sizeof(Table) / sizeof(Table[0]); i++){
      if(Name == *(uint32_t *)Table[i].Name){
        return Table[i].Scancode;
      }
    }
    return 0;
  }
#endif

MD_Keyboard_Error MD_Keyboard_open(MD_Keyboard_t *Keyboard){
  #if MD_Keyboard_set_backend == 0
    Keyboard->Display = XOpenDisplay(NULL);
    if(!Keyboard->Display){
      return MD_Keyboard_Error_InternalError;
    }
    XkbDescPtr desc = XkbGetMap(Keyboard->Display, 0, XkbUseCoreKbd);
    if(desc == NULL){
      goto gt_Close0;
    }
    if(XkbGetNames(Keyboard->Display, XkbKeyNamesMask, desc) != Success){
      goto gt_Close1;
    }
    for(uint16_t i = 0; i < 0x200; i++){
      Keyboard->ScancodeToKeycode[i] = 0;
    }
    {
      uint16_t MaxKeycode = 0xff;
      if(MaxKeycode > desc->max_key_code){
        MaxKeycode = desc->max_key_code;
      }
      for(uint16_t CurrentKeycode = desc->min_key_code; CurrentKeycode <= MaxKeycode; CurrentKeycode++){
        Keyboard->ScancodeToKeycode[_MD_Keyboard_ScancodeFromXkbDesc(desc, CurrentKeycode)] = CurrentKeycode;
      }
    }
    XkbFreeNames(desc, 0, True);
    XkbFreeKeyboard(desc, 0, True);
    return MD_Keyboard_Error_Success;
    gt_Close1:
    XkbFreeKeyboard(desc, 0, True);
    gt_Close0:
    XCloseDisplay(Keyboard->Display);
    return MD_Keyboard_Error_InternalError;
  #elif MD_Keyboard_set_backend == 1
    Keyboard->Connection = xcb_connect(NULL, NULL);
    int err = xcb_connection_has_error(Keyboard->Connection);
    if(err){
      xcb_disconnect(Keyboard->Connection);
      return MD_Keyboard_Error_InternalError;
    }

    return MD_Keyboard_Error_Success;
  #elif MD_Keyboard_set_backend == 2
    return MD_Keyboard_Error_Success;
  #endif
}
void MD_Keyboard_close(MD_Keyboard_t *Keyboard){
  #if MD_Keyboard_set_backend == 0
    XCloseDisplay(Keyboard->Display);
  #elif MD_Keyboard_set_backend == 1
    xcb_disconnect(Keyboard->Connection);
  #elif MD_Keyboard_set_backend == 2
    return;
  #endif
}

MD_Keyboard_Error MD_Keyboard_WriteKey(MD_Keyboard_t *Keyboard, uint16_t Scancode, uint8_t State){
  #if MD_Keyboard_set_backend == 0
    uint16_t isc = ((Scancode & 0xff00) ? 0x0100 : 0) | (Scancode & 0x00ff);
    uint8_t Keycode = Keyboard->ScancodeToKeycode[isc];
    if(Keycode == 0){
      return MD_Keyboard_Error_UnknownArgument;
    }
    int r = XTestFakeKeyEvent(Keyboard->Display, Keycode, State ? True : False, CurrentTime);
    if(r == 0){
      return MD_Keyboard_Error_Temporary;
    }
    XFlush(Keyboard->Display);
    return MD_Keyboard_Error_Success;
  #elif MD_Keyboard_set_backend == 1
    /*
    xcb_keysym_t ks = *Character;
    xcb_keycode_t *kc = xcb_key_symbols_get_keycode(Keyboard->KeySymbols, ks);
    if(!kc){
      return MD_Keyboard_Error_InternalError;
    }
    xcb_generic_error_t *err;
    xcb_void_cookie_t cookie;
    cookie = xcb_test_fake_input(Keyboard->Connection, XCB_KEY_PRESS, *kc, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
    err = xcb_request_check(Keyboard->Connection, cookie);
    if(err){
      return MD_Keyboard_Error_InternalError;
    }
    cookie = xcb_test_fake_input(Keyboard->Connection, XCB_KEY_RELEASE, *kc, XCB_CURRENT_TIME, XCB_NONE, 0, 0, 0);
    err = xcb_request_check(Keyboard->Connection, cookie);
    if(err){
      return MD_Keyboard_Error_InternalError;
    }
    xcb_flush(Keyboard->Connection);
    */
    return MD_Keyboard_Error_Success;
  #elif MD_Keyboard_set_backend == 2
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0;
    input.ki.wScan = Scancode & 0x00ff;
    input.ki.dwFlags = KEYEVENTF_SCANCODE | ((Scancode & 0xff00) ? KEYEVENTF_EXTENDEDKEY : 0) | (State ? 0 : KEYEVENTF_KEYUP);
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    UINT uSent = SendInput(1, &input, sizeof(INPUT));
    if(uSent != 1){
      return MD_Keyboard_Error_Temporary;
    }
    return MD_Keyboard_Error_Success;
  #endif
}

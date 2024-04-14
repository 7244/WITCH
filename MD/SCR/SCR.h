#pragma once

#include _WITCH_PATH(MD/MD.h)

typedef struct{
  uint32_t x;
  uint32_t y;
}MD_SCR_Resolution_t;
typedef struct{
  MD_SCR_Resolution_t Resolution;
  uint32_t LineSize;
}MD_SCR_Geometry_t;

#if MD_set_backend == 0
  #if MD_API_set_backend == 0
    #include _WITCH_PATH(MD/SCR/backend/xorg/xlib/xlib.h)
  #elif MD_API_set_backend == 1
    #include _WITCH_PATH(MD/SCR/backend/xorg/xcb/xcb.h)
  #else
    #error MD_API_set_backend is not defined
  #endif
#elif MD_set_backend == 1
  #include _WITCH_PATH(MD/SCR/backend/DXGI/DXGI.h)
#else
  #error MD_set_backend is not defined
#endif

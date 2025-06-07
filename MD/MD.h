#pragma once

/*
  0 xorg
    0 xlib
    1 xcb
  1 windows
  2 wayland
*/

#ifndef MD_set_backend
  #if defined(__platform_unix)
    // This macro is defined when building
    #if defined(__HAVE_WAYLAND)
      #define MD_set_backend 2
    #else
      #define MD_set_backend 0
    #endif
  #elif defined(__platform_windows)
    #define MD_set_backend 1
  #endif
#endif

#ifndef MD_API_set_backend
  #if MD_set_backend == 0
    #define MD_API_set_backend 1
  #elif MD_set_backend == 2
    #define MD_API_set_backend 0
  #endif
#endif

#if MD_set_backend == 0
  #if MD_API_set_backend == 1
    #include <xcb/xcb.h>

    xcb_screen_t *_MD_GetScreenByIndex(const xcb_setup_t *setup, uint32_t screen_num){
      xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
      while(it.rem > 0){
        if(!screen_num){
          return it.data;
        }
        screen_num--;
        xcb_screen_next(&it);
      }
      return 0;
    }
  #endif
#elif MD_set_backend == 2
  #include <wayland-client.h>
  #include "wlr-screencopy-unstable-v1-client-protocol.h"
  #include "xdg-output-unstable-v1-client-protocol.h"
  
  // Wayland doesn't have a direct screen index concept like X11
  // Outputs are discovered through registry enumeration
  typedef struct {
    struct wl_output *output;
    uint32_t id;
    char *name;
    int32_t x, y;
    int32_t width, height;
  } MDWaylandOutput;
  
  // Helper to get output by index (would need full implementation)
  struct wl_output *MDGetOutputByIndex(struct wl_display *display, uint32_t output_num) {
    // This would require implementing full registry enumeration
    // and maintaining an output list - placeholder for now
    return NULL;
  }
#endif

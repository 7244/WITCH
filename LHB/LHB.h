#pragma once

/* LHB functions
  is_node_invalid = gives true if node is invalid
  open = opens LHB_t struct user must close it after open.
  close = closes LHB_t struct.
  lo = give low bound
  hi = give high bound
  link_next = links value to next
  link_prev = links value to prev
  link_src = links value to src
  link_dst = links value to dst
  link = link value to sorted place
  out_number = gives number of node
  out = gives data pointer of node
*/

#include _WITCH_PATH(WITCH.h)

#ifndef LHB_set_backend
  #if WL_CPP && WITCH_LIBCPP
    #define LHB_set_backend 0
  #else
    #define LHB_set_backend 0
  #endif
#endif

#if LHB_set_backend == 0
  #include _WITCH_PATH(LHB/backend/0/LHB.h)
#elif LHB_set_backend == 1
  #error not very implemented
  #include _WITCH_PATH(LHB/backend/1/LHB.h)
#else
  #error ?
#endif

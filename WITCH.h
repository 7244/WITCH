#pragma once

#define EXPAND(p0) p0
#define ARG(...) __VA_ARGS__
#define STR(_m) #_m
#ifndef CONCAT
  #define _WITCH_CONCAT(p0, p1) p0 ## p1
  #define CONCAT(p0, p1) _WITCH_CONCAT(p0, p1)
#endif
#ifndef CONCAT2
  #define _WITCH_CONCAT2(p0, p1) p0 ## p1
  #define CONCAT2(p0, p1) _WITCH_CONCAT2(p0, p1)
#endif
#ifndef CONCAT3
  #define _WITCH_CONCAT3(p0, p1, p2) p0 ## p1 ## p2
  #define CONCAT3(p0, p1, p2) _WITCH_CONCAT3(p0, p1, p2)
#endif
#ifndef CONCAT4
  #define _WITCH_CONCAT4(p0, p1, p2, p3) p0 ## p1 ## p2 ## p3
  #define CONCAT4(p0, p1, p2, p3) _WITCH_CONCAT4(p0, p1, p2, p3)
#endif

#ifndef WITCH_INCLUDE_PATH
  #define _WITCH_PATH(p0) <WITCH/p0>
#else
  #define _WITCH_PATH(p0) <WITCH_INCLUDE_PATH/p0>
#endif

#include _WITCH_PATH(internal/COMPILER.h)
#include _WITCH_PATH(internal/sysinfo.h)
#include _WITCH_PATH(internal/BIT.h)

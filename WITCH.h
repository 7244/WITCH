#pragma once

#define EXPAND(p0) p0
#define ARG(...) __VA_ARGS__
#define STR(_m) #_m
#define _CONCAT(_0_m, _1_m) _0_m ## _1_m
#define CONCAT(_0_m, _1_m) _CONCAT(_0_m, _1_m)
#define _CONCAT2(_0_m, _1_m) _0_m ## _1_m
#define CONCAT2(_0_m, _1_m) _CONCAT(_0_m, _1_m)
#define _CONCAT3(_0_m, _1_m, _2_m) _0_m ## _1_m ## _2_m
#define CONCAT3(_0_m, _1_m, _2_m) _CONCAT3(_0_m, _1_m, _2_m)
#define _CONCAT4(_0_m, _1_m, _2_m, _3_m) _0_m ## _1_m ## _2_m ## _3_m
#define CONCAT4(_0_m, _1_m, _2_m, _3_m) _CONCAT4(_0_m, _1_m, _2_m, _3_m)

#ifndef WITCH_INCLUDE_PATH
  #define _WITCH_PATH(p0) <WITCH/p0>
#else
  #define _WITCH_PATH(p0) <WITCH_INCLUDE_PATH/p0>
#endif

#include _WITCH_PATH(internal/COMPILER.h)
#include _WITCH_PATH(internal/sysinfo.h)
#include _WITCH_PATH(internal/BIT.h)

#ifndef MAP_set_Backend
  #define MAP_set_Backend 0
#endif

#ifndef MAP_set_Prefix
  #error ifndef MAP_set_Prefix
#endif

#ifndef MAP_set_OutputType
  #error ifndef MAP_set_OutputType
#endif

#ifndef MAP_set_MaxInput
  #define MAP_set_MaxInput ((1 << SYSTEM_BIT - 1) | ((1 << SYSTEM_BIT - 1) - 1))
#endif

#define _MAP_P(p0) CONCAT3(MAP_set_Prefix,_,p0)

#if MAP_set_backend == 0
  #include _WITCH_PATH(MAP/backend/0/0.h)
#else
  #error ?
#endif

#undef _MAP_P

#undef MAP_set_MaxInput

#undef MAP_set_OutputType
#ifdef MAP_set_InputType
  #undef MAP_set_InputType
#endif

#undef MAP_set_Prefix

#undef MAP_set_Backend

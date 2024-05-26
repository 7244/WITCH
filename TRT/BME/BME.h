#ifndef TRT_BME_set_Prefix
  #error ifndef TRT_BME_set_Prefix
#endif

#ifndef TRT_BME_set_Language
  #define TRT_BME_set_Language 0
#endif

#ifndef TRT_BME_set_AreWeInsideStruct
  #define TRT_BME_set_AreWeInsideStruct 0
#endif

#ifndef TRT_BME_set_StructFormat
  #if TRT_BME_set_Language == 0
    #define TRT_BME_set_StructFormat 0
  #elif TRT_BME_set_Language == 1
    #define TRT_BME_set_StructFormat 1
  #else
    #error ?
  #endif
#endif

/*
  0: linux
  1: windows, winapi
  2: c++
*/
#ifndef TRT_BME_set_Backend
  #if defined(WOS_WINDOWS)
    #define TRT_BME_set_Backend 1
  #else
    #define TRT_BME_set_Backend 0
  #endif
#endif

#ifndef TRT_BME_set_MutexType
  #define TRT_BME_set_MutexType 0
#endif

#ifndef TRT_BME_set_LockValue
  #define TRT_BME_set_LockValue 0
#endif
#if TRT_BME_set_LockValue != 0 && TRT_BME_set_LockValue != 1
  #error ?
#endif

#define _TRT_BME_P(p) CONCAT3(TRT_BME_set_Prefix,_,p)

#if TRT_BME_set_StructFormat == 0
  #define TRT_BME_StructBegin(n) typedef struct{
  #define TRT_BME_StructEnd(n) }n;
#elif TRT_BME_set_StructFormat == 1
  #define TRT_BME_StructBegin(n) struct n{
  #define TRT_BME_StructEnd(n) };
#else
  #error ?
#endif

#if TRT_BME_set_Language == 0
  #define _TRT_BME_POFTWBIT(p0) _P(p0)
  #define _TRT_BME_SOFTWBIT static
  #define _TRT_BME_PTFF t
  #define _TRT_BME_PTFFC _TRT_BME_PTFF,
  #define _TRT_BME_DTFF _P(t) *_TRT_BME_PTFF
  #define _TRT_BME_DTFFC _TRT_BME_DTFF,
  #define _TRT_BME_GetType _TRT_BME_PTFF
  #define _TRT_BME_OCITFFE ,
#elif TRT_BME_set_Language == 1
  #define _TRT_BME_POFTWBIT(p0) p0
  #define _TRT_BME_SOFTWBIT
  #define _TRT_BME_PTFF
  #define _TRT_BME_PTFFC
  #define _TRT_BME_DTFF
  #define _TRT_BME_DTFFC
  #define _TRT_BME_GetType this
  #define _TRT_BME_OCITFFE
#endif

#include "rest.h"

#undef _TRT_BME_POFTWBIT
#undef _TRT_BME_SOFTWBIT
#undef _TRT_BME_PTFF
#undef _TRT_BME_PTFFC
#undef _TRT_BME_DTFF
#undef _TRT_BME_DTFFC
#undef _TRT_BME_GetType
#undef _TRT_BME_OCITFFE

#undef TRT_BME_StructEnd
#undef TRT_BME_StructBegin

#undef _TRT_BME_P

#ifdef TRT_BME_set_Conditional
  #undef TRT_BME_set_Conditional
#endif
#undef TRT_BME_set_LockValue
#undef TRT_BME_set_MutexType
#undef TRT_BME_set_Backend
#undef TRT_BME_set_StructFormat
#undef TRT_BME_set_AreWeInsideStruct
#undef TRT_BME_set_Language
#undef TRT_BME_set_Prefix

#pragma once

#ifndef ETC_WED_set_Prefix
  #error set Prefix
#endif

#ifndef ETC_WED_set_WidthType
  #define ETC_WED_set_WidthType uint32_t
#endif

#ifndef ETC_WED_set_debug_InvalidCharacterAccess
  #define ETC_WED_set_debug_InvalidCharacterAccess 0
#endif
#ifndef ETC_WED_set_debug_InvalidLineAccess
  #define ETC_WED_set_debug_InvalidLineAccess 0
#endif
#ifndef ETC_WED_set_debug_InvalidCursorAccess
  #define ETC_WED_set_debug_InvalidCursorAccess 0
#endif

#ifndef ETC_WED_set_Abort
  #define ETC_WED_set_Abort() assert(0)
#endif

#ifndef ETC_WED_set_DataType
  #define ETC_WED_set_DataType wchar_t
#endif

#define _ETC_WED_P(p) CONCAT3(ETC_WED_set_Prefix,_,p)

#include "internal/Base.h"

#undef _ETC_WED_P

#undef ETC_WED_set_DataType

#undef ETC_WED_set_Abort

#undef ETC_WED_set_debug_InvalidCursorAccess
#undef ETC_WED_set_debug_InvalidLineAccess
#undef ETC_WED_set_debug_InvalidCharacterAccess

#undef ETC_WED_set_WidthType

#undef ETC_WED_set_Prefix

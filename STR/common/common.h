#pragma once

#ifndef STR_common_set_backend
  #if defined(__platform_libc)
    #define STR_common_set_backend 0
  #else
    #define STR_common_set_backend 1
  #endif
#endif

#if STR_common_set_backend == 0
  #include _WITCH_PATH(STR/common/backend/0/0.h)
#elif STR_common_set_backend == 1
  #include _WITCH_PATH(STR/common/backend/1/1.h)
#else
  #error ?
#endif

static sint32_t _STR_ParseCStringAsBool(const void *cstr, bool *val){
  if(!STR_casecmp("0", cstr)){
    *val = 0;
  }
  else if(!STR_casecmp("1", cstr)){
    *val = 1;
  }
  else if(!STR_casecmp("false", cstr)){
    *val = 0;
  }
  else if(!STR_casecmp("true", cstr)){
    *val = 1;
  }
  else{
    return 1;
  }

  return 0;
}

static bool STR_ParseCStringAsBool_abort(const void *cstr){
  bool ret;
  if(_STR_ParseCStringAsBool(cstr, &ret)){
    __abort();
  }

  return ret;
}

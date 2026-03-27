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
  else if(!STR_casecmp("no", cstr)){
    *val = 0;
  }
  else if(!STR_casecmp("yes", cstr)){
    *val = 1;
  }
  else if(!STR_casecmp("n", cstr)){
    *val = 0;
  }
  else if(!STR_casecmp("y", cstr)){
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

/* returns how many skip left */
static
uintptr_t
STR_GetIndexAfterSkipNXCharacters_safe(
  const void *ptr,
  uintptr_t *ptr_index,
  uintptr_t ptr_size,
  uintptr_t skip_amount,
  uint8_t character
){
  uint8_t *byte_ptr = (uint8_t *)ptr;

  while(skip_amount){
    if(*ptr_index >= ptr_size){
      break;
    }

    if(byte_ptr[*ptr_index] == character){
      skip_amount--;
    }

    (*ptr_index)++;
  }

  return skip_amount;
}

static
uintptr_t
STR_FindCharacterIndexN_safe(
  const void *ptr,
  uintptr_t *ptr_index,
  uintptr_t ptr_size,
  uintptr_t hit_amount,
  uint8_t character
){
  uint8_t *byte_ptr = (uint8_t *)ptr;

  if(!hit_amount){
    return 0;
  }

  while(1){
    if(*ptr_index >= ptr_size){
      break;
    }

    if(byte_ptr[*ptr_index] == character){
      hit_amount--;

      if(!hit_amount){
        break;
      }
    }

    (*ptr_index)++;
  }

  return hit_amount;
}

/* returns false if not possible. */
/* to needs to be size of 12 bytes */
static bool STR_ExtractPCIAddressInsideString(const uint8_t *str, uintptr_t size, uint8_t *to){
  uintptr_t i = size - 1;
  while(i != (uintptr_t)-1){
    if(str[i] == '/'){
      i++;
      break;
    }
    i--;
  }
  if(i == (uintptr_t)-1){
    return false;
  }

  uint8_t check[] = {0,0,0,0,1,0,0,1,0,0,2,0};
  if(size - i != sizeof(check)){
    return false;
  }

  for(uintptr_t c = 0; c < sizeof(check); c++){
    if(check[c] == 0){
      if(!STR_ischar_hexdigit(str[i + c])){
        return false;
      }
    }
    else if(check[c] == 1){
      if(str[i + c] != ':'){
        return false;
      }
    }
    else if(check[c] == 2){
      if(str[i + c] != '.'){
        return false;
      }
    }
  }

  __builtin_memcpy(to, &str[i], sizeof(check));

  return true;
}

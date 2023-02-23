#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(MEM/MEM.h)

#include _WITCH_PATH(STR/common/common.h)
#include _WITCH_PATH(STR/pss.h)
#include _WITCH_PATH(STR/psu.h)
#include _WITCH_PATH(STR/psh.h)
#include _WITCH_PATH(STR/psf.h)

uint8_t STR_is_char_same_with_any_char_inside_str(uint8_t c, uint8_t *str, uintptr_t str_size){
  for(uintptr_t i = 0; i < str_size; i++)
    if(c == str[i])
      return 1;
  return 0;
}
#define STR_is_char_same_with_any_char_inside_str(c_m, str_m, str_size_m) \
  STR_is_char_same_with_any_char_inside_str((uint8_t)(c_m), (uint8_t *)(str_m), (uintptr_t)(str_size_m))

uint8_t *STR_search_char(uint8_t *str, uintptr_t str_size, uint8_t c, uint8_t *stop_str){
  uint8_t *str_end = str + str_size;
  uintptr_t stop_str_size = MEM_cstreu(stop_str);

  for(; str < str_end; str++){
    if(*str == c)
      return str;
    else if(STR_is_char_same_with_any_char_inside_str(*str, stop_str, stop_str_size))
      return 0;
  }
  return 0;
}
#define STR_search_char(str_m, str_size_m, c_m, stop_str_m) \
  STR_search_char((uint8_t *)(str_m), (uintptr_t)(str_size_m), (uint8_t)(c_m), (uint8_t *)(stop_str_m))
#define STR_pui_search_char(str_m, c_m, stop_str_m) \
  STR_search_char((str_m).ptr, (str_m).uint, c_m, stop_str_m)

uint8_t *STR_search_char_end(uint8_t *str, uint8_t *str_end, uint8_t c, uint8_t *stop_str){
  uintptr_t stop_str_size = MEM_cstreu(stop_str);

  for(; str < str_end; str++){
    if(*str == c)
      return str;
    else if(STR_is_char_same_with_any_char_inside_str(*str, stop_str, stop_str_size))
      return 0;
  }
  return 0;
}
#define STR_search_char_end(str_m, str_end_m, c_m, stop_str_m) \
  STR_search_char_end((uint8_t *)(str_m), (uint8_t *)(str_end_m), (uint8_t)(c_m), (uint8_t *)(stop_str_m))

uint8_t STR_is_char_empty(uint8_t c){
  return STR_is_char_same_with_any_char_inside_str(c, " \r\n", 3);
}
#define STR_is_char_empty(c_m) \
  STR_is_char_empty((uint8_t)(c_m))

uintptr_t STR_size_of_word(uint8_t *str_begin, uint8_t *str_end){
  uint8_t *str = str_begin;
  for(; str < str_end; str++)
    if(STR_is_char_empty(*str))
      break;
  return str - str_begin;
}
#define STR_size_of_word(str_begin_m, str_end_m) \
  STR_size_of_word((uint8_t *)(str_begin_m), (uint8_t *)(str_end_m))

uint8_t *STR_seek_to_line(uint8_t *str_begin, uint8_t *str_end, uintptr_t nline){
  for(uintptr_t iline = 0; iline < nline;){
    if(str_begin >= str_end)
      return 0;
    if(*str_begin == '\n')
      iline++;
    str_begin++;
  }
  return str_begin;
}
#define STR_seek_to_line(str_begin_m, str_end_m, nline_m) \
  STR_seek_to_line((uint8_t *)(str_begin_m), (uint8_t *)(str_end_m), (uintptr_t)(nline_m))
#define STR_index_to_line(str_begin_m, size_m, nline_m) \
  (STR_seek_to_line(str_begin_m, &(str_begin_m)[size_m], nline_m) - (str_begin_m))

uintptr_t STR_how_many_word(uint8_t *str, uintptr_t str_size, uintptr_t nline){
  uint8_t *str_end = str + str_size;

  if(!(str = STR_seek_to_line(str, str_end, nline)))
    return (uintptr_t)~0;

  uintptr_t iword = 0;
  for(uint8_t round = 1; str < str_end && *str != '\n';){
    iword += round && !STR_is_char_empty(*str);
    round = STR_is_char_empty(*str);
    str++;
  }

  return iword;
}
#define STR_how_many_word(str_m, str_size_m, nline_m) \
  STR_how_many_word((uint8_t *)(str_m), (uintptr_t)(str_size_m), (uintptr_t)(nline_m))

uintptr_t STR_get_size_of_line(uint8_t *str_begin, uint8_t *str_end){
  uintptr_t last = 0, i = 0;
  for(; &str_begin[i] < str_end; i++){
    if(str_begin[i] == '\n')
      break;
    if(!STR_is_char_empty(str_begin[i]))
      last = i + 1;
  }
  return last;
}
#define STR_get_size_of_line(str_begin_m, str_end_m) \
  STR_get_size_of_line((uint8_t *)(str_begin_m), (uint8_t *)(str_end_m))

uintptr_t STR_get_empty_line(uint8_t *str, uintptr_t str_size){
  for(uintptr_t iline = 0; ; iline++)
    switch(STR_how_many_word(str, str_size, iline)){
    case (uintptr_t)0:
      return iline;
    case (uintptr_t)~0:
      return ~0;
    }
}
#define STR_get_empty_line(str_m, str_size_m) \
  STR_get_empty_line((uint8_t *)(str_m), (uintptr_t)(str_size_m))

#include _WITCH_PATH(STR/rscan.h)
#include _WITCH_PATH(STR/ttcc.h)

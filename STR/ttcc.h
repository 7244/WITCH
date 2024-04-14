#pragma once

#include _WITCH_PATH(STR/common/common.h)
#include _WITCH_PATH(STR/psu.h)
#include _WITCH_PATH(STR/uto.h)
#include _WITCH_PATH(STR/sto.h)
#ifndef WITCH_float_is_disabled
  #include _WITCH_PATH(STR/fto.h)
#endif

typedef struct STR_ttcc_t STR_ttcc_t;
struct STR_ttcc_t{
  uint8_t *ptr;
  uintptr_t c, p;
  bool (*f)(STR_ttcc_t *);
  void *arg;
}; /* type that can call */

static bool _STR_ttcc_com(STR_ttcc_t *ttcc, const bool side, uint64_t pad, const uint8_t padchar, bool firstdatafirstaspad, const uint8_t *data, uintptr_t size){
  if(pad <= size){
    pad = 0;
    goto gt;
  }
  pad -= size;
  if(!side){
    goto gt;
  }
  if(firstdatafirstaspad && pad && size){
    if(ttcc->c == ttcc->p && ttcc->f(ttcc)){
      return 1;
    }
    ttcc->ptr[ttcc->c++] = *data++;
    size--;
  }
  while(pad){
    uintptr_t left = ttcc->p - ttcc->c;
    uintptr_t n = left < pad ? left : pad;
    MEM_set(padchar, &ttcc->ptr[ttcc->c], n);
    ttcc->c += n;
    if(ttcc->c == ttcc->p && ttcc->f(ttcc)){
      return 1;
    }
    pad -= n;
  }
  gt:
  while(size){
    uintptr_t left = ttcc->p - ttcc->c;
    uintptr_t n = left < size ? left : size;
    MEM_copy(data, &ttcc->ptr[ttcc->c], n);
    ttcc->c += n;
    if(ttcc->c == ttcc->p && ttcc->f(ttcc)){
      return 1;
    }
    data += n;
    size -= n;
  }
  while(pad){
    uintptr_t left = ttcc->p - ttcc->c;
    uintptr_t n = left < pad ? left : pad;
    MEM_set(padchar, &ttcc->ptr[ttcc->c], n);
    ttcc->c += n;
    if(ttcc->c == ttcc->p && ttcc->f(ttcc)){
      return 1;
    }
    pad -= n;
  }
  return 0;
}

static bool STR_vFSttcc(STR_ttcc_t *ttcc, const char *cstr, va_list argv){
  const char *cstr_end = (const char *)MEM_cstrep(cstr);

  while(cstr < cstr_end){
    uintptr_t pvalue[2] = {0, (uintptr_t)-1};
    bool psign[2] = {1, 1};
    uint8_t pfiller[2] = {' ', 0};
    uint8_t l = 0;
    uintptr_t cp = 0;
    if(*cstr != '%'){
      goto normal;
    }
    cstr++;
    if(cstr == cstr_end){
      break;
    }
    if(*cstr == '%'){
      goto normal;
    }
    for(; cstr < cstr_end; cstr++){
      begin:;
      switch(*cstr){
        case '.':{
          cp++;
          if(cp == 2){
            return 1;
          }
          break;
        }
        case '+':{
          psign[cp] = 1;
          break;
        }
        case '-':{
          psign[cp] = 0;
          break;
        }
        case '0':{
          pfiller[cp] = '0';
          break;
        }
        case '1':case '2':case '3':
        case '4':case '5':case '6':
        case '7':case '8':case '9':{
          uintptr_t index = 0;
          pvalue[cp] = STR_psu_iguess(cstr, &index);
          cstr += index;
          goto begin;
        }
        case '*':{
          sintptr_t var = va_arg(argv, sintptr_t);
          psign[cp] = SIGN(var) == 1 ? 1 : 0;
          pvalue[cp] = RSIGN(var);
          break;
        }
        case 'l':{
          l++;
          break;
        }
        case 'x':{
          if(l > 2){
            return 1;
          }
          uint8_t data[128];
          uintptr_t size;
          uint8_t *_data = data;
          switch(l){
            case 0:{
              if(STR_uto(va_arg(argv, uintptr_t), 16, &_data, &size)){
                return 1;
              }
              break;
            }
            case 1:{
              if(STR_uto32(va_arg(argv, uint32_t), 16, &_data, &size)){
                return 1;
              }
              break;
            }
            case 2:{
              if(STR_uto64(va_arg(argv, uint64_t), 16, &_data, &size)){
                return 1;
              }
              break;
            }
          }
          if(_STR_ttcc_com(ttcc, psign[0], pvalue[0], pfiller[0], 0, _data, size)){
            return 1;
          }
          cstr++;
          goto exitop;
        }
        case 'u':{
          if(l > 2){
            return 1;
          }
          uint8_t data[128];
          uintptr_t size;
          uint8_t *_data = data;
          switch(l){
            case 0:{
              if(STR_uto(va_arg(argv, uintptr_t), 10, &_data, &size)){
                return 1;
              }
              break;
            }
            case 1:{
              if(STR_uto32(va_arg(argv, uint32_t), 10, &_data, &size)){
                return 1;
              }
              break;
            }
            case 2:{
              if(STR_uto64(va_arg(argv, uint64_t), 10, &_data, &size)){
                return 1;
              }
              break;
            }
          }
          if(_STR_ttcc_com(ttcc, psign[0], pvalue[0], pfiller[0], 0, _data, size)){
            return 1;
          }
          cstr++;
          goto exitop;
        }
        case 'd':{
          if(l > 2){
            return 1;
          }
          uint8_t data[128];
          uintptr_t size;
          uint8_t *_data = data;
          switch(l){
            case 0:{
              if(STR_sto(va_arg(argv, sintptr_t), 10, &_data, &size))
                return 1;
              break;
            }
            case 1:{
              if(STR_sto32(va_arg(argv, sint32_t), 10, &_data, &size))
                return 1;
              break;              
            }
            case 2:{
              if(STR_sto64(va_arg(argv, sint64_t), 10, &_data, &size))
                return 1;
              break;
            }
          }
          if(_STR_ttcc_com(ttcc, psign[0], pvalue[0], pfiller[0], STR_ischar_digit(!*_data) && pfiller[0] == '0', _data, size)){
            return 1;
          }
          cstr++;
          goto exitop;
        }
        #ifndef WITCH_float_is_disabled
          case 'f':{
            if(l > 3)
              return 1;
            uint8_t data[128];
            if(pvalue[1] == (uintptr_t)-1){
              pvalue[1] = 6;
            }
            uintptr_t size;
            switch(l){
              case 0:
              case 1:
              case 2:{
                if(STR_fto128_base10(va_arg(argv, f64_t), pvalue[1], data, &size))
                  return 1;
                break;
              }
              case 3:{
                if(STR_fto128_base10(va_arg(argv, f128_t), pvalue[1], data, &size))
                  return 1;
                break;
              }
            }
            if(_STR_ttcc_com(ttcc, psign[0], pvalue[0], pfiller[0], STR_ischar_digit(!*data) && pfiller[0] == '0', data, size))
              return 1;
            cstr++;
            goto exitop;
          }
        #endif
        case 's':{
          const uint8_t *str = va_arg(argv, const uint8_t *);
          uintptr_t size = pvalue[1] != (uintptr_t)-1 ? pvalue[1] : MEM_cstreu(str);
          if(_STR_ttcc_com(ttcc, psign[0], pvalue[0], pfiller[0], 0, str, size))
            return 1;
          cstr++;
          goto exitop;
        }
        case 'c':{
          if(ttcc->c == ttcc->p && ttcc->f(ttcc))
            return 1;
          ttcc->ptr[ttcc->c++] = (uint8_t)va_arg(argv, uint32_t);
          cstr++;
          goto exitop;
        }
      }
    }
    exitop:
    continue;
    normal:
      if(ttcc->c == ttcc->p && ttcc->f(ttcc)){
        return 1;
      }
      ttcc->ptr[ttcc->c++] = *cstr++;
  }
  return 0;
}

static bool STR_FSttcc(STR_ttcc_t *ttcc, const char *cstr_begin, ...){
  va_list argv;
  va_start(argv, cstr_begin);
  bool r = STR_vFSttcc(ttcc, cstr_begin, argv);
  va_end(argv);
  return r;
}

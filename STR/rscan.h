#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(STR/pss.h)
#include _WITCH_PATH(STR/common/common.h)

typedef uint8_t *(*STR_rscan_fcb_t)(void *, uintptr_t *);

enum{
  _STR_rscan_type_unknown,
  _STR_rscan_type_any,
  _STR_rscan_type_char,
  _STR_rscan_type_clist,
  _STR_rscan_type_signed,
  _STR_rscan_type_unsigned,
  _STR_rscan_type_hex,
  _STR_rscan_type_float
};

typedef struct{
  uintptr_t src, dst;
}_STR_rscan_range_t;

typedef struct{
  uint32_t type;
  uint8_t out;
  uintptr_t outsize;
  _STR_rscan_range_t range;
  union{
    uint8_t c;
    struct{
      uint8_t clist[0xff];
      uint8_t clistsize;
    };
  };
}_STR_rscan_type_t;

typedef struct{
  uint32_t type;
  uint8_t out;
  uintptr_t outsize;
  union{
    uint64_t snumber;
    uint64_t unumber;
    f64_t f;
    const uint8_t *outdata;
  };
}_STR_rscan_out_t;

bool _STR_rscan_get_range_param(const uint8_t *in, uintptr_t *i, const uintptr_t size, _STR_rscan_range_t *r){
  r->src = 1;
  r->dst = -1;
  if(STR_ischar_digit(in[*i])){
    r->src = STR_psu64_i(in, i, size);
    if(in[*i] != '-'){
      r->dst = r->src;
      r->src = 1;
      return 0;
    }
  }
  else if(in[*i] == '-')
    r->src = 0;
  else
    return 0;
  (*i)++;
  if(*i >= size)
    return 0;
  if(STR_ischar_digit(in[*i]))
    r->dst = STR_psu64_i(in, i, size);
  return 0;
}

_STR_rscan_type_t _STR_rscan_get_operator(const uint8_t *in, uintptr_t *i, const uintptr_t size){
  _STR_rscan_type_t r;
  r.type = _STR_rscan_type_unknown;
  (*i)++;
  if(*i >= size)
    return r;
  if(in[*i] == 'o'){
    (*i)++;
    if(*i >= size)
      return r;
    switch(in[*i]){
      case 'v':
        r.out = 1;
        break;
      case 'c':
        r.out = 2;
        break;
      default:
        return r;
    }
    (*i)++;
    if(*i >= size)
      return r;
    if(STR_ischar_digit(in[*i])){
      r.outsize = STR_psu64_i(in, i, size);
      if(*i >= size)
        return r;
    }
    else r.outsize = -1;
  }
  else r.out = 0;
  (*i)++;
  if(*i >= size)
    return r;
  switch(in[(*i) - 1]){
    case '*':{
      _STR_rscan_get_range_param(in, i, size, &r.range);
      if(*i >= size)
        return r;
      if(in[*i] == 'l'){
        bool escape = 0;
        r.clistsize = 0;
        do{
          (*i)++;
          if(*i >= size)
            return r;
          if(escape){
            escape = 0;
            goto normal;
          }
          if(in[*i] == '\\'){
            escape = 1;
            continue;
          }
          else if(in[*i] == ')')
            break;
          normal:
            r.clist[r.clistsize] = in[*i];
            r.clistsize++;
        }while(1);
        if(r.clistsize == 1){
          r.type = _STR_rscan_type_char;
          r.c = r.clist[0];
        }
        else
          r.type = _STR_rscan_type_clist;
      }
      else if(in[*i] == ')')
        r.type = _STR_rscan_type_any;
      (*i)++;
      break;
    }
    case 's':{
      _STR_rscan_get_range_param(in, i, size, &r.range);
      if(*i >= size)
        return r;
      if(in[*i] != ')')
        return r;
      (*i)++;
      r.type = _STR_rscan_type_signed;
      break;
    }
    case 'u':{
      _STR_rscan_get_range_param(in, i, size, &r.range);
      if(*i >= size)
        return r;
      if(in[*i] != ')')
        return r;
      (*i)++;
      r.type = _STR_rscan_type_unsigned;
      break;
    }
    case 'h':{
      _STR_rscan_get_range_param(in, i, size, &r.range);
      if(*i >= size)
        return r;
      if(in[*i] != ')')
        return r;
      (*i)++;
      r.type = _STR_rscan_type_hex;
      break;
    }
    case 'f':{
      _STR_rscan_get_range_param(in, i, size, &r.range);
      if(*i >= size)
        return r;
      if(in[*i] != ')')
        return r;
      (*i)++;
      r.type = _STR_rscan_type_float;
      break;
    }
    default:
      r.type = _STR_rscan_type_unknown;
      break;
  }
  if(r.outsize == -1)
    switch(r.type){
      case _STR_rscan_type_signed:
      case _STR_rscan_type_unsigned:
      case _STR_rscan_type_hex:
      case _STR_rscan_type_float:
        r.outsize = SYSTEM_BIT;
    }
  return r;
}

_STR_rscan_type_t _STR_rscan_get_type(const uint8_t *in, uintptr_t *i, const uintptr_t size){
  _STR_rscan_type_t r;
  r.type = _STR_rscan_type_unknown;
  r.out = 0;
  bool escape = 0;
  for(; *i < size;){
    if(escape)
      goto normal;
    if(in[*i] == '\\'){
      (*i)++;
      escape = 1;
      continue;
    }
    if(in[*i] == '('){
      r = _STR_rscan_get_operator(in, i, size);
      break;
    }
    normal:
      r.type = _STR_rscan_type_char;
      r.c = in[*i];
      (*i)++;
      break;
  }
  return r;
}

_STR_rscan_out_t _STR_rscan_apply_regex_any(const uint8_t *da, uintptr_t *dai, const uintptr_t dasize, _STR_rscan_type_t *t, const uint8_t *re, uintptr_t *rei, const uintptr_t resize){
  _STR_rscan_out_t r;
  r.outdata = &da[*dai];
  _STR_rscan_type_t next;
  uintptr_t trei = *rei;
  while(1){
    next = _STR_rscan_get_type(re, &trei, resize);
    if(next.type != _STR_rscan_type_any)
      break;
    *rei = trei;
  }
  switch(next.type){
    case _STR_rscan_type_unknown:{
      *dai = dasize;
      break;
    }
    case _STR_rscan_type_char:{
      for(; *dai < dasize; (*dai)++){
        if(da[*dai] == next.c)
          break;
      }
      break;
    }
    case _STR_rscan_type_clist:{
      for(; *dai < dasize; (*dai)++){
        for(uintptr_t ci = 0; ci < next.clistsize; ci++){
          if(da[*dai] == next.clist[ci])
            goto clist_break;
        }
      }
      clist_break:
      break;
    }
    case _STR_rscan_type_signed:{
      for(; *dai < dasize; (*dai)++){
        if(STR_ischar_beginofsigned(da[*dai]))
          break;
      }
      break;
    }
    case _STR_rscan_type_unsigned:{
      for(; *dai < dasize; (*dai)++){
        if(STR_ischar_digit(da[*dai]))
          break;
      }
      break;
    }
    case _STR_rscan_type_hex:{
      uintptr_t hi = 0;
      for(; *dai < dasize; (*dai)++){
        hi = STR_ischars_hex(da[*dai], 0) * (hi + 1);
        if(hi == 2){
          (*dai) -= 2;
          break;
        }
      }
      break;
    }
    case _STR_rscan_type_float:{
      for(; *dai < dasize; (*dai)++){
        if(STR_ischar_float(da[*dai]))
          break;
      }
      break;
    }
  }
  r.type = _STR_rscan_type_any;
  r.outsize = (uintptr_t)(&da[*dai] - r.outdata);
  return r;
}

_STR_rscan_out_t _STR_rscan_apply_regex(const uint8_t *da, uintptr_t *dai, const uintptr_t dasize, const uint8_t *re, uintptr_t *rei, const uintptr_t resize){
  _STR_rscan_out_t r;
  r.type = _STR_rscan_type_unknown;
  uintptr_t _rei = *rei;
  _STR_rscan_type_t t = _STR_rscan_get_type(re, rei, resize);
  switch(t.type){
    case _STR_rscan_type_unknown:
      return r;
    case _STR_rscan_type_any:{
      r = _STR_rscan_apply_regex_any(da, dai, dasize, &t, re, rei, resize);
      break;
    }
    case _STR_rscan_type_char:{
      r.outdata = &da[*dai];
      for(; *dai < dasize; (*dai)++){
        if(da[*dai] != t.c)
          break;
      }
      r.outsize = (uintptr_t)(&da[*dai] - r.outdata);
      break;
    }
    case _STR_rscan_type_clist:{
      r.outdata = &da[*dai];
      for(; *dai < dasize; (*dai)++){
        for(uintptr_t ci = 0; ci < t.clistsize; ci++){
          if(da[*dai] != t.clist[ci])
            goto clist_break;
        }
      }
      clist_break:
      r.outsize = (uintptr_t)(&da[*dai] - r.outdata);
      break;
    }
    case _STR_rscan_type_signed:{
      uintptr_t _dai = *dai;
      r.snumber = STR_pss64_i(da, dai, dasize);
      if(_dai != *dai)
        r.outsize = t.outsize;
      else{
        t.type = _STR_rscan_type_unknown;
        *rei = _rei;
      }
      break;
    }
    case _STR_rscan_type_unsigned:{
      uintptr_t _dai = *dai;
      r.unumber = STR_psu64_i(da, dai, dasize);
      if(_dai != *dai)
        r.outsize = t.outsize;
      else{
        t.type = _STR_rscan_type_unknown;
        *rei = _rei;
      }
      break;
    }
    case _STR_rscan_type_hex:{
      uintptr_t _dai = *dai;
      r.unumber = STR_psh64_i(da, dai, dasize);
      if(_dai != *dai)
        r.outsize = t.outsize;
      else{
        t.type = _STR_rscan_type_unknown;
        *rei = _rei;
      }
      break;
    }
    case _STR_rscan_type_float:{
      uintptr_t _dai = *dai;
      r.f = STR_psf64_i(da, dai, dasize);
      if(_dai != *dai)
        r.outsize = t.outsize;
      else{
        t.type = _STR_rscan_type_unknown;
        *rei = _rei;
      }
      break;
    }
  }
  r.type = t.type;
  r.out = t.out;
  return r;
}

bool STR_vrscanss(const uint8_t *da, uintptr_t *dai, const uintptr_t dasize, const uint8_t *re, const uintptr_t resize, va_list argv){
  uintptr_t daistack = 0;
  if(!dai)
    dai = &daistack;
  uintptr_t rei = 0;
  for(; rei < resize;){
    _STR_rscan_out_t apply = _STR_rscan_apply_regex(da, dai, dasize, re, &rei, resize);
    if(apply.type == _STR_rscan_type_unknown)
      break;
    switch(apply.out){
      case 0:
        break;
      case 1:{
        void *pvar = va_arg(argv, void *);
        switch(apply.type){
          case _STR_rscan_type_char:
            *(uint8_t *)pvar = apply.outdata[0];
            break;
          case _STR_rscan_type_any:
          case _STR_rscan_type_clist:
            MEM_copy(apply.outdata, pvar, apply.outsize);
            break;
          case _STR_rscan_type_signed:
          case _STR_rscan_type_unsigned:
          case _STR_rscan_type_hex:
            switch(apply.outsize){
              case 8:
                *(uint8_t *)pvar = apply.unumber;
                break;
              case 16:
                *(uint16_t *)pvar = apply.unumber;
                break;
              case 32:
                *(uint32_t *)pvar = apply.unumber;
                break;
              case 64:
                *(uint64_t *)pvar = apply.unumber;
                break;
            }
            break;
          case _STR_rscan_type_float:{
            switch(apply.outsize){
              case 32:
                *(f32_t *)pvar = apply.f;
                break;
              case 64:
                *(f64_t *)pvar = apply.f;
                break;
            }
          }
        }
        break;
      }
      case 2:{
        void *fcb = va_arg(argv, void *);
        void *pvar = va_arg(argv, void *);
        uint8_t *out = ((STR_rscan_fcb_t)fcb)(pvar, &apply.outsize);
        MEM_copy(apply.outdata, out, apply.outsize);
        break;
      }
    }
    if(*dai == dasize)
      break;
  }
  return rei != resize;
}
#define STR_vrscanss(da_m, dai_m, dasize_m, re_m, resize_m, argv_m) \
  STR_vrscanss((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uintptr_t)(dasize_m), (const uint8_t *)(re_m), (const uintptr_t)(resize_m), argv_m)
bool STR_vrscansc(const uint8_t *da, uintptr_t *dai, const uintptr_t dasize, const uint8_t *re, va_list argv){
  return STR_vrscanss(da, dai, dasize, re, MEM_cstreu(re), argv);
}
#define STR_vrscansc(da_m, dai_m, dasize_m, re_m, argv_m) \
  STR_vrscansc((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uintptr_t)(dasize_m), (const uint8_t *)(re_m), argv_m)
bool STR_vrscancs(const uint8_t *da, uintptr_t *dai, const uint8_t *re, const uintptr_t resize, va_list argv){
  return STR_vrscanss(da, dai, MEM_cstreu(da), re, resize, argv);
}
#define STR_vrscancs(da_m, dai_m, re_m, resize_m, argv_m) \
  STR_vrscancs((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uint8_t *)(re_m), (const uintptr_t)(resize_m), argv_m)
bool STR_vrscancc(const uint8_t *da, uintptr_t *dai, const uint8_t *re, va_list argv){
  return STR_vrscanss(da, dai, MEM_cstreu(da), re, MEM_cstreu(re), argv);
}
#define STR_vrscancc(da_m, dai_m, re_m, resize_m, argv_m) \
  STR_vrscancc((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uint8_t *)(re_m), (const uintptr_t)(resize_m), argv_m)
bool STR_rscanss(const uint8_t *da, uintptr_t *dai, const uintptr_t dasize, const uint8_t *re, const uintptr_t resize, ...){
  va_list argv;
  va_start(argv, resize);
  bool r = STR_vrscanss(da, dai, dasize, re, resize, argv);
  va_end(argv);
  return r;
}
#define STR_rscanss(da_m, dai_m, dasize_m, re_m, resize_m, ...) \
  STR_rscanss((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uintptr_t)(dasize_m), (const uint8_t *)(re_m), (const uintptr_t)(resize_m), ##__VA_ARGS__)
bool STR_rscansc(const uint8_t *da, uintptr_t *dai, const uintptr_t dasize, const uint8_t *re, ...){
  va_list argv;
  va_start(argv, re);
  bool r = STR_vrscanss(da, dai, dasize, re, MEM_cstreu(re), argv);
  va_end(argv);
  return r;
}
#define STR_rscansc(da_m, dai_m, dasize_m, re_m, ...) \
  STR_rscansc((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uintptr_t)(dasize_m), (const uint8_t *)(re_m), ##__VA_ARGS__)
bool STR_rscancs(const uint8_t *da, uintptr_t *dai, const uint8_t *re, const uintptr_t resize, ...){
  va_list argv;
  va_start(argv, resize);
  bool r = STR_vrscanss(da, dai, MEM_cstreu(da), re, resize, argv);
  va_end(argv);
  return r;
}
#define STR_rscancs(da_m, dai_m, re_m, resize_m, ...) \
  STR_rscancs((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uint8_t *)(re_m), (const uintptr_t)(resize_m), ##__VA_ARGS__)
bool STR_rscancc(const uint8_t *da, uintptr_t *dai, const uint8_t *re, ...){
  va_list argv;
  va_start(argv, re);
  bool r = STR_vrscanss(da, dai, MEM_cstreu(da), re, MEM_cstreu(re), argv);
  va_end(argv);
  return r;
}
#define STR_rscancc(da_m, dai_m, re_m, ...) \
  STR_rscancc((const uint8_t *)(da_m), (uintptr_t *)(dai_m), (const uint8_t *)(re_m), ##__VA_ARGS__)
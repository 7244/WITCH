#pragma once

#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(MEM/MEM.h)

typedef struct{
  uintptr_t Current, Possible, Type, Buffer;
  uint8_t *ptr;
  A_resize_t resize;
}VEC_t;
static void VEC_init(VEC_t *vec, uintptr_t size, A_resize_t resize){
  vec->Current = 0;
  vec->Possible = 0;
  vec->Type = size;
  vec->Buffer = _A_calculate_buffer(size);
  vec->ptr = 0;
  vec->resize = resize;
}

static void VEC_reserve(VEC_t *vec, uintptr_t Amount){
  vec->Possible = Amount;
  vec->ptr = vec->resize(vec->ptr, vec->Possible * vec->Type);
}

static void _VEC_handle(VEC_t *vec){
  vec->Possible = vec->Current + vec->Buffer;
  vec->ptr = vec->resize(vec->ptr, vec->Possible * vec->Type);
}
static void VEC_handle(VEC_t *vec){
  if(vec->Current >= vec->Possible){
    _VEC_handle(vec);
  }
}
static void VEC_handle0(VEC_t *vec, uintptr_t amount){
  vec->Current += amount;
  VEC_handle(vec);
}

static void VEC_dupe(VEC_t *src, VEC_t *dst){
  dst->Current = src->Current;
  dst->Possible = 0;
  dst->Type = src->Type;
  dst->Buffer = src->Buffer;
  dst->ptr = 0;
  dst->resize = src->resize;
  VEC_handle(dst);
  MEM_copy(src->ptr, dst->ptr, dst->Current * dst->Type);
}

static void VEC_free(VEC_t *vec){
  vec->resize(vec->ptr, 0);
  vec->Current = 0;
  vec->Possible = 0;
  vec->ptr = 0;
}

typedef struct{
  const uint8_t *ptr;
  uintptr_t size;
  uint8_t _step;
}VEC_export_t;
static void VEC_export_init(VEC_export_t *arg){
  arg->_step = 0;
}
static bool VEC_export(VEC_export_t *arg, const VEC_t *vec){
  switch(arg->_step){
    case 0:{
      arg->ptr = (const uint8_t *)&vec->Current;
      arg->size = sizeof(vec->Current);
      break;
    }
    case 1:{
      arg->ptr = (const uint8_t *)&vec->Type;
      arg->size = sizeof(vec->Type);
      break;
    }
    case 2:{
      arg->ptr = (const uint8_t *)vec->ptr;
      arg->size = vec->Current * vec->Type;
      break;
    }
    case 3:{
      return 0;
    }
  }
  arg->_step++;
  return 1;
}
typedef struct{
  uint8_t *ptr;
  uintptr_t size;
  uint8_t _step;
}VEC_import_t;
static void VEC_import_init(VEC_import_t *arg){
  arg->_step = 0;
}
static bool VEC_import(VEC_import_t *arg, VEC_t *vec){
  switch(arg->_step){
    case 0:{
      arg->ptr = (uint8_t *)&vec->Possible;
      arg->size = sizeof(vec->Possible);
      break;
    }
    case 1:{
      arg->ptr = (uint8_t *)&vec->Type;
      arg->size = sizeof(vec->Type);
      break;
    }
    case 2:{
      arg->size = vec->Possible * vec->Type;
      vec->ptr = A_resize(0, arg->size);
      arg->ptr = vec->ptr;
      break;
    }
    case 3:{
      vec->Current = vec->Possible;
      vec->Buffer = _A_calculate_buffer(vec->Type);
      vec->resize = A_resize;
      return 0;
    }
  }
  arg->_step++;
  return 1;
}

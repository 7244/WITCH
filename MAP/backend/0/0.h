#ifdef MAP_set_InputType
  #define _MAP_b_GetInputParameter \
    MAP_set_InputType *i
  #define _MAP_b_ExpandInput \
    i
#else
  #define _MAP_b_GetInputParameter \
    const void *i, \
    uintptr_t InputSize
  #define _MAP_b_ExpandInput \
    i, InputSize
#endif

#if MAP_set_MaxInput <= 0xff
  typedef uint8_t _MAP_P(_nri_t);
#elif MAP_set_MaxInput <= 0xffff
  typedef uint16_t _MAP_P(_nri_t);
#elif MAP_set_MaxInput <= 0xffffffff
  typedef uint32_t _MAP_P(_nri_t);
#elif MAP_set_MaxInput <= 0xffffffffffffffff
  typedef uint64_t _MAP_P(_nri_t);
#endif

#pragma pack(push, 1)
  typedef struct{
    #ifdef MAP_set_InputType
      MAP_set_InputType i;
    #else
      void *i;
      uintptr_t InputSize;
    #endif
    MAP_set_OutputType o;
  }_MAP_P(_nd_t);
#pragma pack(pop)

#define BLL_set_prefix _MAP_P(_List)
#define BLL_set_Language 0
#define BLL_set_type_node _MAP_P(_nri_t)
#define BLL_set_NodeDataType _MAP_P(_nd_t)
#include <BLL/BLL.h>

typedef _MAP_P(_List_NodeReference_t) _MAP_P(nr_t);

typedef struct{
  _MAP_P(_List_t) List;
}_MAP_P(t);

void
_MAP_P(Open)(
  _MAP_P(t) *t
){
  _MAP_P(_List_Open)(&t->List);
}
void
_MAP_P(Close)(
  _MAP_P(t) *t
){
  _MAP_P(_List_Close)(&t->List);
}

_MAP_P(nr_t)
_MAP_P(QueryInput)(
  _MAP_P(t) *t,
  _MAP_b_GetInputParameter
){
  _MAP_P(nr_t) nr = _MAP_P(_List_GetNodeFirst)(&t->List);
  for(
    _MAP_P(_List_Node_t) *n;
    !_MAP_P(_List_inre)(nr, t->List.dst);
    nr = n->NextNodeReference
  ){
    n = _MAP_P(_List_GetNodeByReference)(&t->List, nr);
    #ifdef MAP_set_InputType
      uintptr_t InputSize = sizeof(MAP_set_InputType);
      const void *ni = &n->data.i;
    #else
      if(InputSize != n->data.InputSize){
        continue;
      }
      const void *ni = n->data.i;
    #endif
    if(!MEM_cmp(ni, i, InputSize)){
      break;
    }
  }
  if(_MAP_P(_List_inre)(nr, t->List.dst)){
    return _MAP_P(_List_gnric)();
  }
  return nr;
}

bool
_MAP_P(IsNRInvalid)(
  _MAP_P(t) *t,
  _MAP_P(nr_t) nr
){
  return _MAP_P(_List_inric)(nr);
}

bool
_MAP_P(DoesInputExists)(
  _MAP_P(t) *t,
  _MAP_b_GetInputParameter
){
  _MAP_P(nr_t) nr = _MAP_P(QueryInput)(t, _MAP_b_ExpandInput);
  return !_MAP_P(IsNRInvalid)(t, nr);
}

MAP_set_OutputType *
_MAP_P(GetOutputPointerSafe)(
  _MAP_P(t) *t,
  _MAP_b_GetInputParameter
){
  _MAP_P(nr_t) nr = _MAP_P(QueryInput)(t, _MAP_b_ExpandInput);
  if(_MAP_P(IsNRInvalid)(t, nr)){
    return NULL;
  }
  _MAP_P(_List_Node_t) *n = _MAP_P(_List_GetNodeByReference)(&t->List, nr);
  return &n->data.o;
}

MAP_set_OutputType *
_MAP_P(GetOutputPointer)(
  _MAP_P(t) *t,
  _MAP_b_GetInputParameter
){
  _MAP_P(nr_t) nr = _MAP_P(QueryInput)(t, _MAP_b_ExpandInput);
  _MAP_P(_List_Node_t) *n = _MAP_P(_List_GetNodeByReference)(&t->List, nr);
  return &n->data.o;
}

void
_MAP_P(InNew)(
  _MAP_P(t) *t,
  _MAP_b_GetInputParameter,
  MAP_set_OutputType *o
){
  _MAP_P(nr_t) nr = _MAP_P(_List_NewNodeLast)(&t->List);
  _MAP_P(_List_Node_t) *n = _MAP_P(_List_GetNodeByReference)(&t->List, nr);
  #ifdef MAP_set_InputType
    n->data.i = *i;
  #else
    n->data.InputSize = InputSize;
    n->data.i = A_resize(NULL, n->data.InputSize);
    __builtin_memcpy(n->data.i, i, InputSize);
  #endif
  n->data.o = *o;
}

void
_MAP_P(Remove)(
  _MAP_P(t) *t,
  _MAP_b_GetInputParameter
){
  _MAP_P(nr_t) nr = _MAP_P(QueryInput)(t, _MAP_b_ExpandInput);
  #ifndef MAP_set_InputType
    _MAP_P(_List_Node_t) *n = _MAP_P(_List_GetNodeByReference)(&t->List, nr);
    A_resize(n->data.i, 0);
  #endif
  _MAP_P(_List_unlrec)(&t->List, nr);
}

#ifdef MAP_set_InputType
  typedef struct{
    _MAP_P(_List_NodeReference_t) nr;
  }_MAP_P(Iterate_t);
  void _MAP_P(Iterate_Open)(
    _MAP_P(t) *t,
    _MAP_P(Iterate_t) *it
  ){
    it->nr = _MAP_P(_List_GetNodeFirst)(&t->List);
  }
  void _MAP_P(Iterate_Close)(
    _MAP_P(t) *t,
    _MAP_P(Iterate_t) *it
  ){
    /* ~soul who lost his path~ */
  }
  MAP_set_OutputType *_MAP_P(Iterate)(
    _MAP_P(t) *t,
    _MAP_P(Iterate_t) *it,
    _MAP_b_GetInputParameter
  ){
    if(_MAP_P(_List_inre)(it->nr, t->List.dst)){
      return NULL;
    }

    _MAP_P(_List_Node_t) *n = _MAP_P(_List_GetNodeByReference)(&t->List, it->nr);
    *i = n->data.i;
    it->nr = n->NextNodeReference;
    return &n->data.o;
  }
#endif

#undef _MAP_b_ExpandInput
#undef _MAP_b_GetInputParameter

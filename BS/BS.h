/* can be used for debug internal code inside with checking preprocessor */
#ifndef BS_set_Mark
  #define BS_set_Mark 0
#endif

#ifndef BS_set_BaseLibrary
  #define BS_set_BaseLibrary 0
#endif

#ifndef BS_set_Language
  #if BS_set_BaseLibrary == 0
    #define BS_set_Language 0
  #elif BS_set_BaseLibrary == 1
    #define BS_set_Language 1
  #else
    #error ?
  #endif
#endif

#ifndef BS_set_AreWeInsideStruct
  #define BS_set_AreWeInsideStruct 0
#endif

#ifndef BS_set_prefix
  #error ifndef BS_set_prefix
#endif
#ifndef BS_set_StructFormat
  #if BS_set_Language == 0
    #define BS_set_StructFormat 0
  #elif BS_set_Language == 1
    #define BS_set_StructFormat 1
  #else
    #error ?
  #endif
#endif
#ifndef BS_set_declare_rest
  #define BS_set_declare_rest 1
#endif
#ifndef BS_set_ResizeListAfterClear
  #define BS_set_ResizeListAfterClear 0
#endif
#ifndef BS_set_UseUninitialisedValues
  #if BS_set_BaseLibrary == 0
    #define BS_set_UseUninitialisedValues WITCH_set_UseUninitialisedValues
  #elif BS_set_BaseLibrary == 1
    #define BS_set_UseUninitialisedValues fan_use_uninitialized
  #endif
#endif
#ifndef BS_set_NodeType
  #define BS_set_NodeType uint32_t
#endif

#ifndef BS_set_Backend
  #define BS_set_Backend 0
#endif

#if BS_set_BaseLibrary == 0
  #define _BS_INCLUDE _WITCH_PATH
#elif BS_set_BaseLibrary == 1
  #define _BS_INCLUDE _FAN_PATH
#endif

#define _BS_P(p0) CONCAT3(BS_set_prefix, _, p0)

#if BS_set_StructFormat == 0
  #define BS_StructBegin(n) typedef struct{
  #define BS_StructEnd(n) }n;
#elif BS_set_StructFormat == 1
  #define BS_StructBegin(n) struct n{
  #define BS_StructEnd(n) };
#endif

#if BS_set_declare_rest == 1
  /* _BS_POFTWBIT; prefix of function that would be inside type */
  /* _BS_SOFTWBIT; settings of function that would be inside type */
  /* _BS_PBSTFF; pass bs type for functon */
  /* _BS_PBSTFFC; _BS_PBSTFF but with comma */
  /* _BS_DBSTFF; declare bs type for functon */
  /* _BS_DBSTFFC; _BS_DBSTFF but with comma */
  /* _BS_OCIBSTFFE; only comma if bs type for function exists */
  #if BS_set_Language == 0
    #define _BS_POFTWBIT(p0) _P(p0)
    #define _BS_SOFTWBIT static
    #define _BS_PBSTFF _pList
    #define _BS_PBSTFFC _BS_PBSTFF,
    #define _BS_DBSTFF _P(t) *_BS_PBSTFF
    #define _BS_DBSTFFC _BS_DBSTFF,
    #define _BS_GetList _BS_PBSTFF
    #define _BS_OCIBSTFFE ,
  #elif BS_set_Language == 1
    #define _BS_POFTWBIT(p0) p0
    #define _BS_SOFTWBIT
    #define _BS_PBSTFF
    #define _BS_PBSTFFC
    #define _BS_DBSTFF
    #define _BS_DBSTFFC
    #define _BS_GetList this
    #define _BS_OCIBSTFFE
  #endif

  #include "internal/rest.h"

  #undef _BS_POFTWBIT
  #undef _BS_SOFTWBIT
  #undef _BS_PBSTFF
  #undef _BS_PBSTFFC
  #undef _BS_DBSTFF
  #undef _BS_DBSTFFC
  #undef _BS_GetList
  #undef _BS_OCIBSTFFE
#endif

#undef BS_StructBegin
#undef BS_StructEnd

#undef _BS_P

#undef _BS_INCLUDE

#ifdef BS_set_CPP_ConstructDestruct
  #undef BS_set_CPP_ConstructDestruct
#endif
#undef BS_set_Backend
#undef BS_set_NodeType
#undef BS_set_UseUninitialisedValues
#undef BS_set_ResizeListAfterClear
#undef BS_set_StructFormat
#undef BS_set_AreWeInsideStruct
#undef BS_set_prefix
#ifdef BS_set_namespace
  #undef BS_set_namespace
#endif
#undef BS_set_Language
#undef BS_set_BaseLibrary
#undef BS_set_Mark

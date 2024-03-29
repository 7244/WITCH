/* +++ outdated +++ */

#ifdef BDBT_set_declare_basic_types
  #error outdated setting. now it shipped with BDBT_set_declare_rest
#endif

/* --- outdated --- */

#ifndef BDBT_set_BaseLibrary
  #define BDBT_set_BaseLibrary 0
#endif

#ifndef BDBT_set_Language
  #if BDBT_set_BaseLibrary == 0
    #define BDBT_set_Language 0
  #elif BDBT_set_BaseLibrary == 1
    #define BDBT_set_Language 1
  #else
    #error ?
  #endif
#endif

#ifndef BDBT_set_AreWeInsideStruct
  #define BDBT_set_AreWeInsideStruct 0
#endif

#ifndef BDBT_set_prefix
  #error ifndef BDBT_set_prefix
#endif
#ifndef BDBT_set_StructFormat
  #if BDBT_set_Language == 0
    #define BDBT_set_StructFormat 0
  #elif BDBT_set_Language == 1
    #define BDBT_set_StructFormat 1
  #else
    #error ?
  #endif
#endif
#ifndef BDBT_set_declare_rest
  #define BDBT_set_declare_rest 1
#endif
#ifndef BDBT_set_declare_Key
  #define BDBT_set_declare_Key 1
#endif
#ifndef BDBT_set_PadNode
  #define BDBT_set_PadNode 0
#endif
#ifndef BDBT_set_debug_InvalidAction
  #define BDBT_set_debug_InvalidAction 0
#endif
#ifndef BDBT_set_IsNodeUnlinked
  #if BDBT_set_debug_InvalidAction == 1
    #define BDBT_set_IsNodeUnlinked 1
  #else
    #define BDBT_set_IsNodeUnlinked 0
  #endif
#endif
#ifndef BDBT_set_ResizeListAfterClear
  #define BDBT_set_ResizeListAfterClear 0
#endif
#ifndef BDBT_set_UseUninitialisedValues
  #if BDBT_set_BaseLibrary == 0
    #define BDBT_set_UseUninitialisedValues WITCH_set_UseUninitialisedValues
  #elif BDBT_set_BaseLibrary == 1
    #define BDBT_set_UseUninitialisedValues fan_use_uninitialized
  #endif
#endif
#ifndef BDBT_set_type_node
  #define BDBT_set_type_node uint32_t
#endif
#ifndef BDBT_set_BitPerNode
  #define BDBT_set_BitPerNode 2
#endif

#if BDBT_set_BitPerNode == 1
  #define _BDBT_set_ElementPerNode 0x2
#elif BDBT_set_BitPerNode == 2
  #define _BDBT_set_ElementPerNode 0x4
#elif BDBT_set_BitPerNode == 4
  #define _BDBT_set_ElementPerNode 0x10
#elif BDBT_set_BitPerNode == 8
  #define _BDBT_set_ElementPerNode 0x100
#else
  #error ?
#endif

#if BDBT_set_BaseLibrary == 0
  #define _BDBT_INCLUDE _WITCH_PATH
#elif BDBT_set_BaseLibrary == 1
  #define _BDBT_INCLUDE _FAN_PATH
#endif

#ifdef BDBT_set_base_prefix
  #define _BDBT_BP(p0) CONCAT3(BDBT_set_base_prefix, _, p0)
#else
  #define _BDBT_BP(p0) CONCAT3(BDBT_set_prefix, _, p0)
#endif
#define _BDBT_P(p0) CONCAT3(BDBT_set_prefix, _, p0)

#if BDBT_set_StructFormat == 0
  #define BDBT_StructBegin(n) typedef struct{
  #define BDBT_StructEnd(n) }n;
#elif BDBT_set_StructFormat == 1
  #define BDBT_StructBegin(n) struct n{
  #define BDBT_StructEnd(n) };
#endif

#if BDBT_set_declare_rest == 1
  #include _BDBT_INCLUDE(BDBT/internal/rest.h)
#endif
#if BDBT_set_declare_Key == 1
  #include _BDBT_INCLUDE(BDBT/internal/Key/Key.h)
#endif

#undef BDBT_StructBegin
#undef BDBT_StructEnd

#undef _BDBT_P
#undef _BDBT_BP

#undef _BDBT_INCLUDE

#undef _BDBT_set_ElementPerNode
#undef BDBT_set_BitPerNode
#ifdef BDBT_set_CPP_ConstructDestruct
  #undef BDBT_set_CPP_ConstructDestruct
#endif
#undef BDBT_set_prefix
#undef BDBT_set_declare_Key
#undef BDBT_set_declare_rest
#undef BDBT_set_type_node
#undef BDBT_set_PadNode
#undef BDBT_set_debug_InvalidAction
#undef BDBT_set_IsNodeUnlinked
#undef BDBT_set_ResizeListAfterClear
#undef BDBT_set_UseUninitialisedValues
#ifdef BDBT_set_base_namespace
  #undef BDBT_set_base_namespace
#endif
#ifdef BDBT_set_namespace
  #undef BDBT_set_namespace
#endif

#undef BDBT_set_BaseLibrary

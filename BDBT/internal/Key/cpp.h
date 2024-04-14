template <
  #if !defined(BDBT_set_KeySize)
    uintptr_t _KeySize,
  #endif
  bool BitOrderMatters = false
>
struct _BDBT_P(Key_t){

  #if !defined(BDBT_set_KeySize)
    #define _BDBT_ksizeConstexpr constexpr

    typedef std::conditional_t<
      _KeySize <= 0xff,
      uint8_t,
        std::conditional_t<_KeySize <= 0xffff,
        uint16_t,
        uint32_t
      >
    >KeySize_t;

    #define KeySize (KeySize_t)_KeySize
  #else
    #define _BDBT_ksizeConstexpr

    typedef uintptr_t KeySize_t;

    #define KeySize KeySize
  #endif

  #define BeforeLast (KeySize - 8)

  typedef std::conditional_t<
    _BDBT_set_ElementPerNode <= 0xff,
    uint8_t,
      std::conditional_t<_BDBT_set_ElementPerNode <= 0xffff,
      uint16_t,
      uint32_t
    >
  >KeyNodeIterator_t;

  uint8_t ReverseKeyByte(uint8_t p){
    #if BDBT_set_BitPerNode == 1
      return bitswap8(p);
    #elif BDBT_set_BitPerNode == 2
      p = (p & 0xf0) >> 4 | (p & 0x0f) << 4;
      p = (p & 0xcc) >> 2 | (p & 0x33) << 2;
      return p;
    #elif BDBT_set_BitPerNode == 4
      p = (p & 0xf0) >> 4 | (p & 0x0f) << 4;
      return p;
    #elif BDBT_set_BitPerNode == 8
      return p;
    #else
      #error ?
    #endif
  }

  /* add */
  void
  a
  (
    _BDBT_BP(t) *list,
    #if defined(BDBT_set_KeySize)
      KeySize_t KeySize,
    #endif
    const void *Key,
    KeySize_t KeyIndex,
    _BDBT_BP(NodeReference_t) cnr,
    _BDBT_BP(NodeReference_t) Output
  ){
    #include "cpp/a.h"
  }

  /* query */
  void
  q
  (
    _BDBT_BP(t) *list,
    #if defined(BDBT_set_KeySize)
      KeySize_t KeySize,
    #endif
    const void *Key,
    KeySize_t *KeyIndex,
    _BDBT_BP(NodeReference_t) *cnr
  ){
    #include "cpp/q.h"
  }

  KeySize_t
  _r
  (
    _BDBT_BP(t) *list,
    #if defined(BDBT_set_KeySize)
      KeySize_t KeySize,
    #endif
    void *Key,
    _BDBT_BP(NodeReference_t) *cnr
  ){
    #include "cpp/r.h"
  }
  void
  r
  (
    _BDBT_BP(t) *list,
    #if defined(BDBT_set_KeySize)
      KeySize_t KeySize,
    #endif
    void *Key,
    _BDBT_BP(NodeReference_t) cnr
  ){
    _r(
      list,
      #if defined(BDBT_set_KeySize)
        KeySize,
      #endif
      Key,
      &cnr
    );
  }
  /* remove with info */
  KeySize_t
  rwi
  (
    _BDBT_BP(t) *list,
    #if defined(BDBT_set_KeySize)
      KeySize_t KeySize,
    #endif
    void *Key,
    _BDBT_BP(NodeReference_t) *cnr
  ){
    return _r(
      list,
      #if defined(BDBT_set_KeySize)
        KeySize,
      #endif
      Key,
      cnr
    );
  }

  /* give 0 if you want to sort from low, 1 for high. */
  struct Traverse_t{
    KeySize_t Current;
    struct ta_t{
      _BDBT_BP(NodeReference_t) n;
      KeyNodeIterator_t k;
    };
    #ifndef BDBT_set_KeySize
      ta_t ta[KeySize / BDBT_set_BitPerNode];
    #else
      static uintptr_t GetTraverseArraySize(KeySize_t KeySize){
        return KeySize / BDBT_set_BitPerNode;
      }
    #endif

    _BDBT_BP(NodeReference_t) Output;

    /* init */
    template <uint8_t LowHigh = 2>
    void
    i
    (
      #if defined(BDBT_set_KeySize)
        ta_t *ta,
      #endif
      _BDBT_BP(NodeReference_t) rnr
    ){
      Current = 0;
      ta[0].k = LowHigh == 1 ? _BDBT_set_ElementPerNode - 1 : 0;
      ta[0].n = rnr;
    }
    void
    i0
    (
      #if defined(BDBT_set_KeySize)
        ta_t *ta,
      #endif
      _BDBT_BP(NodeReference_t) rnr,
      uint8_t LowHigh = 2
    ){
      if(LowHigh == 0){
        i<0>(
          #if defined(BDBT_set_KeySize)
            ta,
          #endif
          rnr
        );
      }
      else if(LowHigh == 1){
        i<1>(
          #if defined(BDBT_set_KeySize)
            ta,
          #endif
          rnr
        );
      }
      else{
        i<>(
          #if defined(BDBT_set_KeySize)
            ta,
          #endif
          rnr
        );
      }
    }

    /* traverse */
    template <uint8_t LowHigh = 2>
    bool
    t
    (
      _BDBT_BP(t) *list,
      #if defined(BDBT_set_KeySize)
        ta_t *ta,
        KeySize_t KeySize,
      #endif
      void *Key
    ){
      #include "cpp/t.h"
    }

    bool
    t0(
      _BDBT_BP(t) *list,
      #if defined(BDBT_set_KeySize)
        ta_t *ta,
        KeySize_t KeySize,
      #endif
      void *Key,
      uint8_t LowHigh = 2
    ){
      if(LowHigh == 0){
        return t<0>(
          list,
          #if defined(BDBT_set_KeySize)
            ta,
            KeySize,
          #endif
          Key
        );
      }
      else if(LowHigh == 1){
        return t<1>(
          list,
          #if defined(BDBT_set_KeySize)
            ta,
            KeySize,
          #endif
          Key
        );
      }
      else{
        return t<>(
          list,
          #if defined(BDBT_set_KeySize)
            ta,
            KeySize,
          #endif
          Key
        );
      }
    }
  };

  #undef BeforeLast
  #undef KeySize
  #undef _BDBT_ksizeConstexpr
};

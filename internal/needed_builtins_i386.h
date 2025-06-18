#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_clang) && __clang_major__ >= 18 && __clang_major__ <= 19) || \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14)

  typedef union{
    unsigned long long all;
    struct {
      #if ENDIAN == 1
        unsigned low;
        unsigned high;
      #else
        unsigned high;
        unsigned low;
      #endif
    }s;
  }_WITCH_COMPILER_udwords;

  __no_name_mangling
  unsigned long long __udivmoddi4(unsigned long long a, unsigned long long b, unsigned long long *rem){
    const unsigned n_uword_bits = sizeof(unsigned) * 8;
    const unsigned n_udword_bits = sizeof(unsigned long long) * 8;
    _WITCH_COMPILER_udwords n;
    n.all = a;
    _WITCH_COMPILER_udwords d;
    d.all = b;
    _WITCH_COMPILER_udwords q;
    _WITCH_COMPILER_udwords r;
    unsigned sr;
    if (n.s.high == 0) {
      if (d.s.high == 0) {
        if (rem)
          *rem = n.s.low % d.s.low;
        return n.s.low / d.s.low;
      }
      if (rem)
        *rem = n.s.low;
      return 0;
    }
    if (d.s.low == 0) {
      if (d.s.high == 0) {
        if (rem)
          *rem = n.s.high % d.s.low;
        return n.s.high / d.s.low;
      }
      if (n.s.low == 0) {
        if (rem) {
          r.s.high = n.s.high % d.s.high;
          r.s.low = 0;
          *rem = r.all;
        }
        return n.s.high / d.s.high;
      }
      if ((d.s.high & (d.s.high - 1)) == 0){
        if (rem) {
          r.s.low = n.s.low;
          r.s.high = n.s.high & (d.s.high - 1);
          *rem = r.all;
        }
        return n.s.high >> __builtin_ctz(d.s.high);
      }
      sr = __builtin_clz(d.s.high) - __builtin_clz(n.s.high);
      if (sr > n_uword_bits - 2) {
        if (rem)
          *rem = n.all;
        return 0;
      }
      ++sr;
      q.s.low = 0;
      q.s.high = n.s.low << (n_uword_bits - sr);
      r.s.high = n.s.high >> sr;
      r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
    } else {
      if (d.s.high == 0) {
        if ((d.s.low & (d.s.low - 1)) == 0) {
          if (rem)
            *rem = n.s.low & (d.s.low - 1);
          if (d.s.low == 1)
            return n.all;
          sr = __builtin_ctz(d.s.low);
          q.s.high = n.s.high >> sr;
          q.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
          return q.all;
        }
        sr = 1 + n_uword_bits + __builtin_clz(d.s.low) - __builtin_clz(n.s.high);
        if (sr == n_uword_bits) {
          q.s.low = 0;
          q.s.high = n.s.low;
          r.s.high = 0;
          r.s.low = n.s.high;
        } else if (sr < n_uword_bits){
          q.s.low = 0;
          q.s.high = n.s.low << (n_uword_bits - sr);
          r.s.high = n.s.high >> sr;
          r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
        } else{
          q.s.low = n.s.low << (n_udword_bits - sr);
          q.s.high = (n.s.high << (n_udword_bits - sr)) |
                    (n.s.low >> (sr - n_uword_bits));
          r.s.high = 0;
          r.s.low = n.s.high >> (sr - n_uword_bits);
        }
      } else {
        sr = __builtin_clz(d.s.high) - __builtin_clz(n.s.high);
        if (sr > n_uword_bits - 1) {
          if (rem)
            *rem = n.all;
          return 0;
        }
        ++sr;
        q.s.low = 0;
        if (sr == n_uword_bits) {
          q.s.high = n.s.low;
          r.s.high = 0;
          r.s.low = n.s.high;
        } else {
          q.s.high = n.s.low << (n_uword_bits - sr);
          r.s.high = n.s.high >> sr;
          r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
        }
      }
    }
    unsigned carry = 0;
    for (; sr > 0; --sr) {
      r.s.high = (r.s.high << 1) | (r.s.low >> (n_uword_bits - 1));
      r.s.low = (r.s.low << 1) | (q.s.high >> (n_uword_bits - 1));
      q.s.high = (q.s.high << 1) | (q.s.low >> (n_uword_bits - 1));
      q.s.low = (q.s.low << 1) | carry;
      const long long s = (long long)(d.all - r.all - 1) >> (n_udword_bits - 1);
      carry = s & 1;
      r.all -= d.all & s;
    }
    q.all = (q.all << 1) | carry;
    if (rem)
      *rem = r.all;
    return q.all;
  }

  __no_name_mangling
  unsigned long long __umoddi3(unsigned long long a, unsigned long long b) {
    unsigned long long r;
    __udivmoddi4(a, b, &r);
    return r;
  }

  __no_name_mangling
  unsigned long long __udivdi3(unsigned long long a, unsigned long long b) {
    return __udivmoddi4(a, b, 0);
  }
#endif

#if \
  (!defined(__WITCH_DEVELOPER) || !__WITCH_DEVELOPER) || \
  (defined(__compiler_gcc) && __GNUC__ >= 14 && __GNUC__ <= 14)

  __no_name_mangling
  unsigned __popcountsi2(unsigned a){
    a = a - ((a >> 1) & 0x55555555);
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    a = (a + (a >> 4)) & 0x0F0F0F0F;
    a = (a + (a >> 16));
    return (a + (a >> 8)) & 0x0000003F;
  }
#endif

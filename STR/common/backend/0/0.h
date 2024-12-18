#include _WITCH_PATH(MEM/MEM.h)

#include <string.h>

#if defined(__platform_unix)
  /* used for strncasecmp() */
  #include <strings.h>
#endif

static bool STR_ncasecmp(const void *s0, const void *s1, uintptr_t n){
  #if defined(__platform_unix)
    return strncasecmp((const char *)s0, (const char *)s1, n);
  #elif defined(__platform_windows)
    return _strnicmp((const char *)s0, (const char *)s1, n);
  #else
    #error ?
  #endif
}

static bool STR_ncmp(const void *s0, const void *s1, uintptr_t n){
  return !!strncmp((const char *)s0, (const char *)s1, n);
}

/* n is length of first argument */
static bool STR_n0cmp(const void *s0, const void *s1){
  return !!strncmp((const char *)s0, (const char *)s1, MEM_cstreu(s0));
}

/* n is length of first argument */
/* increase s1 with n if success */
static bool STR_n0cmp_inc1(const void *s0, const void **s1){
  uintptr_t n = MEM_cstreu(s0);
  bool r = !!strncmp((const char *)s0, (const char *)*s1, n);
  if(!r){
    *s1 = (const void *)(((uint8_t *)*s1) + n);
  }
  return r;
}

static bool STR_cmp(const void *s0, const void *s1){
  return !!strcmp((const char *)s0, (const char *)s1);
}

static uintptr_t STR_nchri(const uint8_t *str, uintptr_t strsize, uint8_t c, sintptr_t i){
  const uint8_t *origstr = str;
  while(strsize){
    if(*str == c){
      break;
    }
    str += i;
    strsize--;
  }
  if(!strsize){
    return -1;
  }
  return ABS((sintptr_t)(origstr - str));
}

static uintptr_t STR_nchr(const uint8_t *str, uintptr_t strsize, uint8_t c){
  return STR_nchri(str, strsize, c, 1);
}

static bool STR_ischar_digit(const uint8_t c){
  switch(c){
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
      return 1;
  }
  return 0;
}

static bool STR_ischar_beginofsigned(const uint8_t c){
  switch(c){
    case '-':case '+':
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
      return 1;
  }
  return 0;
}

static bool STR_ischar_hexdigit(const uint8_t c){
  switch(c){
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
    case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
    case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
      return 1;
  }
  return 0;
}

static bool STR_ischars_hex(const uint8_t c, const uintptr_t i){
  switch(i){
    case 0:
      return c == '0';
    case 1:
      return c == 'x';
  }
  return STR_ischar_hexdigit(c);
}

static bool STR_ischar_BeginOfFloat(const uint8_t c){
  switch(c){
    case '.':
    case '+':case '-':
    case '0':case '1':case '2':case '3':case '4':
    case '5':case '6':case '7':case '8':case '9':
      return 1;
  }
  return 0;
}

static bool STR_ischar_char(const uint8_t c){
  switch(c){
    case 'A':case 'B':
    case 'C':case 'D':
    case 'E':case 'F':
    case 'G':case 'H':
    case 'I':case 'J':
    case 'K':case 'L':
    case 'M':case 'N':
    case 'O':case 'P':
    case 'Q':case 'R':
    case 'S':case 'T':
    case 'U':case 'V':
    case 'W':case 'X':
    case 'Y':case 'Z':
    case 'a':case 'b':
    case 'c':case 'd':
    case 'e':case 'f':
    case 'g':case 'h':
    case 'i':case 'j':
    case 'k':case 'l':
    case 'm':case 'n':
    case 'o':case 'p':
    case 'q':case 'r':
    case 's':case 't':
    case 'u':case 'v':
    case 'w':case 'x':
    case 'y':case 'z':
      return 1;
  }
  return 0;
}

static bool STR_ischar_blank(uint8_t c){
  switch(c){
    case 0:
    case ' ':
    case '\r':
    case '\n':
      return 1;
  }
  return 0;
}

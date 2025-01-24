#define __WITCH_CPU_SETSIZE 1024
#define __WITCH_NCPUBITS (8 * sizeof(__WITCH_cpu_mask))

typedef uintptr_t __WITCH_cpu_mask;

typedef struct{
  __WITCH_cpu_mask bits[__WITCH_CPU_SETSIZE / __WITCH_NCPUBITS];
}__WITCH_cpu_set_t;

static uint8_t __WITCH_cpuset_countbits(uint8_t v){
  uint8_t s = 0;
  for(; v != 0; s++){
    v &= v - 1;
  }
  return s;
}

static uintptr_t __WITCH_cpuset_cpucount(uintptr_t size, const __WITCH_cpu_set_t *cpu_set){
  uintptr_t r = 0;
  for(uintptr_t i = 0; i < size; i++){
    r += __WITCH_cpuset_countbits(((uint8_t *)cpu_set->bits)[i]);
  }
  return r;
}

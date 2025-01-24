#define __WITCH_CPU_SETSIZE 1024
#define __WITCH_NCPUBITS (8 * sizeof(__WITCH_cpu_mask))

typedef uintptr_t __WITCH_cpu_mask;

typedef struct{
  __WITCH_cpu_mask bits[__WITCH_CPU_SETSIZE / __WITCH_NCPUBITS];
}__WITCH_cpu_set_t;

static uintptr_t __WITCH_cpuset_countbits(__WITCH_cpu_mask v){
  uintptr_t s = 0;
  for(; v != 0; s++){
    v &= v - 1;
  }
  return s;
}

static uintptr_t __WITCH_cpuset_cpucount(const __WITCH_cpu_set_t *cpu_set){
  uintptr_t r = 0;
  for(uintptr_t i = sizeof(__WITCH_cpu_set_t) / sizeof(__WITCH_cpu_mask); i--;){
    r += __WITCH_cpuset_countbits(cpu_set->bits[i]);
  }
  return r;
}

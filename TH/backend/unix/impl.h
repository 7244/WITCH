#include "../_unix_raw/impl.h"

#include _WITCH_PATH(T/T.h)

static void TH_sleep(uintptr_t ns){
  _T_timespec_t timespec;
  timespec.tv_sec = ns / 1000000000;
  timespec.tv_nsec = ns % 1000000000;
  syscall1(__NR_nanosleep, (uintptr_t)&timespec);
}

static void TH_sleepi(uintptr_t ns){
  TH_sleep(ns);
}

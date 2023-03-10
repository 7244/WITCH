#include <stdlib.h>

#ifndef _PR_set_abort_print
  #define _PR_set_abort_print 1
#endif

#if _PR_set_abort_print
  #include <signal.h>

  #if defined(_MSC_VER) && !defined(__clang__)
    #include <iostream>
    #include <stacktrace>

    void _PR_DumpTrace() {
      std::stacktrace st;
      std::cout << st.current();
    }
  #else
    #include <stdio.h>
    #include <dbghelp.h>

    #ifdef _MSC_VER
      #pragma comment(lib, "Dbghelp.lib")
    #endif

    void _PR_DumpTrace() {
      uint16_t i;
      uint16_t frames;
      void *stack[0xff];
      SYMBOL_INFO *symbol;
      HANDLE process;

      SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES);

      process = GetCurrentProcess();

      if (!SymInitialize(process, NULL, TRUE)) {
        int err = GetLastError();
        printf("[_PR_DumpTrace] SymInitialize failed %d", err);
      }

      frames = CaptureStackBackTrace(0, 0xff, stack, NULL);
      symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 1024 * sizeof(uint8_t), 1);
      symbol->MaxNameLen = 1023;
      symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

      for (i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        DWORD Displacement;
        IMAGEHLP_LINE64 Line;
        Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if (SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &Displacement, &Line)) {
          printf("%i: %s:%u\n", frames - i - 1, symbol->Name, Line.LineNumber);
        }
        else {
          printf("%i: %s:0x%x\n", frames - i - 1, symbol->Name, symbol->Address);
        }
      }

      free(symbol);
    }
  #endif

  void _PR_SignalCatcher(int signal) {
    if (signal == SIGINT) {
      return;
    }
    printf("[PR] _PR_SignalCatcher(%d)\n", signal);
    _PR_DumpTrace();
  }

  PRE{
    typedef void (*SignalHandlerPointer)(int);
    SignalHandlerPointer PreviousHandler;
    PreviousHandler = signal(SIGABRT, _PR_SignalCatcher);
  }
#endif

void PR_exit(uint32_t num){
  exit(num);
}

#if _PR_set_abort_print
  void PR_abort(void) {
    printf("[PR] PR_abort() is called\n");
    _PR_DumpTrace();
    PR_exit(1);
  }
#else
  void PR_abort(void) {
    PR_exit(1);
  }
#endif

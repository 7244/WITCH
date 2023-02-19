#if TRT_BME_set_Backend > 1
  #error ?
#endif

struct _TRT_BME_P(t);

TRT_BME_StructBegin(_TRT_BME_P(t))
  #if TRT_BME_set_Backend == 0
    #if defined(TRT_BME_set_Conditional)
      pthread_mutex_t mutex;
      pthread_cond_t cond;
    #else
      #if TRT_BME_set_MutexType == 0
        pthread_mutex_t mutex;
      #elif TRT_BME_set_MutexType == 1
        #if defined(__compiler_clang) || defined(__compiler_gcc)
          uint8_t value;
        #elif defined(__x86_64__)
          uint8_t value;
        #else
          #error ?
        #endif
      #else
        #error ?
      #endif
    #endif
  #elif TRT_BME_set_Backend == 1
    #if defined(TRT_BME_set_Conditional)
      CRITICAL_SECTION mutex;
      CONDITION_VARIABLE cond;
    #else
      #if TRT_BME_set_MutexType == 0
        CRITICAL_SECTION mutex;
      #elif TRT_BME_set_MutexType == 1
        LONG value;
      #else
        #error ?
      #endif
    #endif
  #endif

  #if TRT_BME_set_Language == 1
    #if TRT_BME_set_MutexType == 0
      _TRT_BME_P(t)(){
        Open();
      }
      ~_TRT_BME_P(t)(){
        Close();
      }
    #elif TRT_BME_set_MutexType == 1
      _TRT_BME_P(t)(){
        Init();
      }
    #else
      #error ?
    #endif
  #endif
#if TRT_BME_set_Language == 0
  TRT_BME_StructEnd(_TRT_BME_P(t))
#endif

_TRT_BME_SOFTWBIT
void
_TRT_BME_POFTWBIT(Lock)(
  _TRT_BME_DTFF
){
  #if TRT_BME_set_Backend == 0
    #if TRT_BME_set_MutexType == 0
      int r = pthread_mutex_lock(&_TRT_BME_GetType->mutex);
      if(r != 0){
        TRT_BME_set_Abort();
      }
    #elif TRT_BME_set_MutexType == 1
      #if defined(__compiler_clang) || defined(__compiler_gcc)
        while(__sync_lock_test_and_set(&_TRT_BME_GetType->value, 1)){
          while(_TRT_BME_GetType->value){
            __builtin_ia32_pause();
          }
        }
      #elif defined(__x86_64__)
        __asm__ __volatile__(
          "mov $1, %%bl\n"
          "Retry%=:\n"
          "xor %%al, %%al\n"
          "XACQUIRE lock cmpxchgb %%bl, (%0)\n"
          "je Done%=\n"
          "Desperate%=:\n"
          "mov (%0), %%al\n"
          "test %%al, %%al\n"
          "jz Retry%=\n"
          "rep nop\n"
          "jmp Desperate%=\n"
          "Done%=:\n"
          :
          : "c" (&_TRT_BME_GetType->value)
          : "al", "bl", "memory"
        );
      #else
        #error ?
      #endif
    #else
      #error ?
    #endif
  #elif TRT_BME_set_Backend == 1
    #if TRT_BME_set_MutexType == 0
      EnterCriticalSection(&_TRT_BME_GetType->mutex);
    #elif TRT_BME_set_MutexType == 1
      while(InterlockedExchange(&_TRT_BME_GetType->value, 1) == 1){

      }
    #else
      #error ?
    #endif
  #endif
}
_TRT_BME_SOFTWBIT
void
_TRT_BME_POFTWBIT(Unlock)(
  _TRT_BME_DTFF
){
  #if TRT_BME_set_Backend == 0
    #if TRT_BME_set_MutexType == 0
      int r = pthread_mutex_unlock(&_TRT_BME_GetType->mutex);
      if(r != 0){
        TRT_BME_set_Abort();
      }
    #elif TRT_BME_set_MutexType == 1
      #if defined(__compiler_clang) || defined(__compiler_gcc)
        __sync_lock_release(&_TRT_BME_GetType->value);
      #elif defined(__x86_64__)
        __asm__ __volatile__(
          "XRELEASE movb $0, (%0)"
          :
          : "r" (&_TRT_BME_GetType->value)
          : "memory"
        );
      #else
        #error ?
      #endif
    #else
      #error ?
    #endif
  #elif TRT_BME_set_Backend == 1
    #if TRT_BME_set_MutexType == 0
      LeaveCriticalSection(&_TRT_BME_GetType->mutex);
    #elif TRT_BME_set_MutexType == 1
      InterlockedExchange(&_TRT_BME_GetType->value, 0);
    #else
      #error ?
    #endif
  #endif
}

#ifdef TRT_BME_set_Conditional
  _TRT_BME_SOFTWBIT
  void
  _TRT_BME_POFTWBIT(Wait)(
    _TRT_BME_DTFF
  ){
    #if TRT_BME_set_Backend == 0
      int r = pthread_cond_wait(&_TRT_BME_GetType->cond, &_TRT_BME_GetType->mutex);
      if(r != 0){
        TRT_BME_set_Abort();
      }
    #elif TRT_BME_set_Backend == 1
      bool r = SleepConditionVariableCS(&_TRT_BME_GetType->cond, &_TRT_BME_GetType->mutex, INFINITE);
      if(r == 0){
        TRT_BME_set_Abort();
      }
    #endif
  }
  _TRT_BME_SOFTWBIT
  void
  _TRT_BME_POFTWBIT(Signal)(
    _TRT_BME_DTFF
  ){
    #if TRT_BME_set_Backend == 0
      int r = pthread_cond_signal(&_TRT_BME_GetType->cond);
      if(r != 0){
        TRT_BME_set_Abort();
      }
    #elif TRT_BME_set_Backend == 1
      WakeConditionVariable(&_TRT_BME_GetType->cond);
    #endif
  }
#endif

#if TRT_BME_set_Language != 0
  private:
#endif
#if TRT_BME_set_MutexType == 0 || defined(TRT_BME_set_Conditional)
  _TRT_BME_SOFTWBIT
  void
  _TRT_BME_POFTWBIT(Open)(
    _TRT_BME_DTFF
  ){
    #if TRT_BME_set_Backend == 0
      int r;
      r = pthread_mutex_init(&_TRT_BME_GetType->mutex, NULL);
      if(r != 0){
        TRT_BME_set_Abort();
      }
      #ifdef TRT_BME_set_Conditional
        r = pthread_cond_init(&_TRT_BME_GetType->cond, NULL);
        if(r != 0){
          TRT_BME_set_Abort();
        }
      #endif
    #elif TRT_BME_set_Backend == 1
      InitializeCriticalSection(&_TRT_BME_GetType->mutex);
      #ifdef TRT_BME_set_Conditional
        InitializeConditionVariable(&_TRT_BME_GetType->cond);
      #endif
    #endif
  }
  _TRT_BME_SOFTWBIT
  void
  _TRT_BME_POFTWBIT(Close)(
    _TRT_BME_DTFF
  ){
    #if TRT_BME_set_Backend == 0
      int r;
      r = pthread_mutex_destroy(&_TRT_BME_GetType->mutex);
      if(r != 0){
        TRT_BME_set_Abort();
      }
      #ifdef TRT_BME_set_Conditional
        r = pthread_cond_destroy(&_TRT_BME_GetType->cond);
        if(r != 0){
          TRT_BME_set_Abort();
        }
      #endif
    #elif TRT_BME_set_Backend == 1
      DeleteCriticalSection(&_TRT_BME_GetType->mutex);
      #ifdef TRT_BME_set_Conditional
        /* it doesnt have to close */
      #endif
    #endif
  }
#elif TRT_BME_set_MutexType == 1
  _TRT_BME_SOFTWBIT
  void
  _TRT_BME_POFTWBIT(Init)(
    _TRT_BME_DTFF
  ){
    #if TRT_BME_set_Backend == 0
      _TRT_BME_GetType->value = 0;
    #elif TRT_BME_set_Backend == 1
      _TRT_BME_GetType->value = 0;
    #else
      #error ?
    #endif
  }
#else
  #error ?
#endif
#if TRT_BME_set_Language != 0
  public:
#endif

#if TRT_BME_set_Language != 0
  TRT_BME_StructEnd(_TRT_BME_P(t))
#endif

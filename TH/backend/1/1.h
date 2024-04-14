static void TH_sleepf(f64_t time){
  Sleep(time * 1000);
}
static void TH_sleepi(uint64_t time){
  Sleep(time / 1000000);
}

typedef HANDLE TH_id_t;

static TH_id_t TH_open(void *f, void *p){
  TH_id_t id;
  if((id = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)f, p, 0, 0)) == NULL){
    return (TH_id_t)-1;
  }
  return id;
}

static bool TH_close(TH_id_t id){
  return CloseHandle(id) == 0;
}

typedef struct{
  CRITICAL_SECTION mutex;
}TH_mutex_t;
static void TH_mutex_init(TH_mutex_t *mutex){
  InitializeCriticalSection(&mutex->mutex);
}

static int TH_lock(TH_mutex_t *mutex){
  EnterCriticalSection(&mutex->mutex);
  return 0;
}

static int TH_unlock(TH_mutex_t *mutex){
  LeaveCriticalSection(&mutex->mutex);
  return 0;
}

typedef struct{
  CONDITION_VARIABLE cond;
  TH_mutex_t mutex;
}TH_cond_t;
static void TH_cond_init(TH_cond_t *cond){
  cond->cond = WITCH_c(CONDITION_VARIABLE)CONDITION_VARIABLE_INIT;
  TH_mutex_init(&cond->mutex);
}

static int TH_wait(TH_cond_t *cond){
  int result = SleepConditionVariableCS(&cond->cond, &cond->mutex.mutex, INFINITE);
  return result;
}

static int TH_twait(TH_cond_t *cond, uint64_t time){
  return SleepConditionVariableCS(&cond->cond, &cond->mutex.mutex, time / 1000000);
}

static int TH_signal(TH_cond_t *cond){
  WakeConditionVariable(&cond->cond);
  return 0;
}

static int TH_join(TH_id_t id){
  return WaitForSingleObject(id, INFINITE);
}

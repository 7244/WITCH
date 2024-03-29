#include <sys/time.h>
#include <pthread.h>

typedef pthread_t TH_id_t;

static void TH_sleepf(f64_t time){
  struct timespec t;
  t.tv_sec = FLOOR(time);
  t.tv_nsec = FMOD(time, 1) * 1000000000;
  nanosleep(&t, &t);
}
static void TH_sleepi(uint64_t time){
  struct timespec t;
  t.tv_sec = time / 1000000000;
  t.tv_nsec = time % 1000000000;
  nanosleep(&t, &t);
}

static TH_id_t TH_open(void *f, void *p){
  TH_id_t id;
  if(pthread_create(&id, 0, (void *(*)(void *))f, p) != 0){
    return (TH_id_t)-1;
  }
  return id;
}

static bool TH_close(TH_id_t id){
  return pthread_cancel(id) != 0;
}
static void TH_close_block(TH_id_t id){
  if(pthread_cancel(id) != 0){
    __abort();
  }
}

typedef struct{
  pthread_mutex_t mutex;
}TH_mutex_t;
static void TH_mutex_init(TH_mutex_t *mutex){
  pthread_mutex_init(&mutex->mutex, NULL);
}

static int TH_lock(TH_mutex_t *mutex){
  return pthread_mutex_lock(&mutex->mutex);
}

static int TH_unlock(TH_mutex_t *mutex){
  return pthread_mutex_unlock(&mutex->mutex);
}

typedef struct{
  pthread_cond_t cond;
  TH_mutex_t mutex;
}TH_cond_t;
static void TH_cond_init(TH_cond_t *cond){
  pthread_cond_init(&cond->cond, NULL);
  TH_mutex_init(&cond->mutex);
}

static int TH_wait(TH_cond_t *cond){
  return pthread_cond_wait(&cond->cond, &cond->mutex.mutex);
}

static int TH_twait(TH_cond_t *cond, uint64_t time){
  struct timeval now;
  gettimeofday(&now, NULL);
  struct timespec t;
  t.tv_sec = now.tv_sec + (time / 1000000000);
  t.tv_nsec = (now.tv_usec * 1000) + (time % 1000000000);
  t.tv_sec += t.tv_nsec / 1000000000;
  t.tv_nsec = t.tv_nsec % 1000000000;
  return pthread_cond_timedwait(&cond->cond, &cond->mutex.mutex, &t);
}

static int TH_signal(TH_cond_t *cond){
  return pthread_cond_signal(&cond->cond);
}

static int TH_join(TH_id_t id){
  return pthread_join(id, 0);
}

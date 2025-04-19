#pragma once

/* concept 0 */

template<typename pType, uint8_t Count = 0>
struct TRT_CON0_t{
private:

  pType *m_Pointer = NULL;

  #define BME_set_Prefix Cond
  #define BME_set_Language 1
  #define BME_set_AreWeInsideStruct 1
  #define BME_set_Conditional 1
  #include <BME/BME.h>
  Cond_t m_Cond;

  uint8_t m_Count = Count;

public:

  bool IsMarkable(){
    return m_Pointer != NULL;
  }
  pType *Mark(){
    m_Cond.Lock();
    if(m_Pointer == NULL){
      m_Cond.Unlock();
      return NULL;
    }
    m_Count++;
    auto r = m_Pointer;
    m_Cond.Unlock();
    return r;
  }
  void Unmark(){
    m_Cond.Lock();
    m_Count--;
    m_Cond.Signal();
    m_Cond.Unlock();
  }
  pType *ReserveMark(){
    m_Cond.Lock();
    auto r = m_Pointer;
    m_Pointer = NULL;
    while(m_Count != 0){
      m_Cond.Wait();
    }
    return r;
  }
  void UnreserveMark(pType *p){
    m_Pointer = p;
    m_Cond.Unlock();
  }
  pType *GetOrphanPointer(){
    return m_Pointer;
  }
  void LocklessSetPointer(pType *p){
    m_Pointer = p;
  }
  void SetPointer(pType *p){
    /* TOOD locks are not needed if assign can be atomic */
    m_Cond.Lock();
    m_Pointer = p;
    m_Cond.Unlock();
  }
};

template <typename Common_t, uint8_t Count = 0>
struct TRT_CON0_Itself_t{
private:

  #define BME_set_Prefix Mutex
  #define BME_set_Language 1
  #define BME_set_AreWeInsideStruct 1
  #include <BME/BME.h>
  Mutex_t m_Mutex;

  uint8_t m_Count = Count;

public:

  void Increase(){
    m_Mutex.Lock();
    m_Count++;
    m_Mutex.Unlock();
  }
  void Decrease(){
    m_Mutex.Lock();
    m_Count--;
    if(m_Count == 0){
      m_Mutex.Unlock();
      delete OFFSETLESS(this, Common_t, Itself);
      return;
    }
    m_Mutex.Unlock();
  }
};

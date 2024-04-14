#ifdef BS_set_namespace
  namespace BS_set_namespace {
#endif

#if BS_set_Backend == 0
  #define BDBT_set_BaseLibrary BS_set_BaseLibrary
  #define BDBT_set_Language 0
  #define BDBT_set_AreWeInsideStruct BS_set_AreWeInsideStruct
  #define BDBT_set_prefix _BS_P(_BDBT)
  #define BDBT_set_declare_rest 1
  #define BDBT_set_declare_Key 1
  #include _BS_INCLUDE(BDBT/BDBT.h)
#endif

BS_StructBegin(_P(t))
  #if BS_set_Backend == 0
  #endif
#if BS_set_Language == 0
  BS_StructEnd(_P(t))
#endif

#ifdef BS_set_CPP_ConstructDestruct
  private:
#endif
_BS_SOFTWBIT
void
_BS_POFTWBIT(Open)
(
  _BS_DBSTFF
){
  
}
_BS_SOFTWBIT
void
_BS_POFTWBIT(Close)
(
  _BS_DBSTFF
){
  
}
#ifdef BS_set_CPP_ConstructDestruct
  public:
#endif

#if BS_set_Language == 1
  #ifdef BS_set_CPP_ConstructDestruct
    _P(t)(){
      Open();
    }
    ~_P(t)(){
      Close();
    }
  #endif

  BS_StructEnd(_P(t))
#endif


#ifdef BS_set_namespace
  }
#endif

struct Contact_Grid_Flag{
  constexpr static uint32_t EnableContact = 0x01;
};

struct Contact_Grid_t{
  uint32_t Flag;
};

typedef void (*PreSolve_Grid_cb_t)(
  __ETC_BCOL_P(t) *,
  ObjectID_t,
  ShapeEnum_t,
  ShapeID_t,
  _vsi32 /* Grid */,
  Contact_Grid_t *
);

#ifdef ETC_BCOL_set_PostSolve_Grid
  struct ContactResult_Grid_t{
    #ifdef ETC_BCOL_set_PostSolve_Grid_CollisionNormal
      _vf Normal;
    #endif
  };

  #ifdef ETC_BCOL_set_PostSolve_Grid_CollisionNormal
    _vf ContactResult_Grid_GetNormal(
      ContactResult_Grid_t *ContactResult
    ){
      return ContactResult->Normal;
    }
  #endif

  typedef void (*PostSolve_Grid_cb_t)(
    __ETC_BCOL_P(t) *,
    ObjectID_t,
    ShapeEnum_t,
    ShapeID_t,
    _vsi32 /* Grid */,
    ContactResult_Grid_t *
  );
#endif

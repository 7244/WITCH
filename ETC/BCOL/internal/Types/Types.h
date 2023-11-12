struct __ETC_BCOL_P(t){
  typedef fan::vec2_wrap_t<uint32_t> _vui32;
  typedef fan::vec2_wrap_t<sint32_t> _vsi32;
  #if ETC_BCOL_set_PreferredFloatSize == 32
    typedef f32_t _f;
    typedef fan::vec2 _vf;
    /*
    __pfloat __floorf(__pfloat p0){ return MATH_floor_f32(p0); }
    __pfloat __sin(__pfloat p0){ return MATH_sin_32(p0); }
    __pfloat __cos(__pfloat p0){ return MATH_cos_32(p0); }
    __pfloat __atan2(__pfloat p0, __pfloat p1){ return MATH_atan2_f32(p0, p1); }
    __pfloat __sqrt(__pfloat p0){ return MATH_sqrt_32(p0); }
    __pfloat __absf(__pfloat p0){ return MATH_abs_f32(p0); }
    __pfloat __hypotenuse(__pfloat p0, __pfloat p1){ return MATH_hypotenuse_f32(p0, p1); }
    void __NormalResolve(
      __pfloat p0, __pfloat p1,
      __pfloat p2, __pfloat p3,
      __pfloat p4,
      __pfloat *p5, __pfloat *p6
      ){ MATH_NormalResolve_f32(p0, p1, p2, p3, p4, p5, p6); }
    __pfloat __dot2(
      __pfloat p0, __pfloat p1,
      __pfloat p2, __pfloat p3
      ){ return MATH_dot2_f32(p0, p1, p2, p3); }
    __pfloat __copysignf(__pfloat p0, __pfloat p1){ return MATH_copysign_f32(p0, p1); }
    */
  #elif ETC_BCOL_set_PreferredFloatSize == 64
    typedef f64_t __pfloat;
    __pfloat __floorf(__pfloat p0){ return MATH_floor_f64(p0); }
    __pfloat __sin(__pfloat p0){ return MATH_sin_64(p0); }
    __pfloat __cos(__pfloat p0){ return MATH_cos_64(p0); }
    __pfloat __atan2(__pfloat p0, __pfloat p1){ return MATH_atan2_f64(p0, p1); }
    __pfloat __sqrt(__pfloat p0){ return MATH_sqrt_64(p0); }
    __pfloat __absf(__pfloat p0){ return MATH_abs_f64(p0); }
    __pfloat __hypotenuse(__pfloat p0, __pfloat p1){ return MATH_hypotenuse_f64(p0, p1); }
    void __NormalResolve(
      __pfloat p0, __pfloat p1,
      __pfloat p2, __pfloat p3,
      __pfloat p4,
      __pfloat *p5, __pfloat *p6
      ){ MATH_NormalResolve_f64(p0, p1, p2, p3, p4, p5, p6); }
    __pfloat __dot2(
      __pfloat p0, __pfloat p1,
      __pfloat p2, __pfloat p3
      ){ return MATH_dot2_f64(p0, p1, p2, p3); }
    __pfloat __copysignf(__pfloat p0, __pfloat p1){ return MATH_copysign_f64(p0, p1); }
  #else
    #error ?
  #endif

  typedef uint32_t ShapeID_ID_t;

  struct ShapeID_t{
    ShapeID_ID_t ID;

    ShapeID_t() = default;
    ShapeID_t(auto p){
      static_assert(
        __is_type_same<ShapeList_Circle_NodeReference_t, decltype(p)> ||
        __is_type_same<ShapeList_Rectangle_NodeReference_t, decltype(p)>);
      this->ID = p.NRI;
    }
  };

  enum class ShapeEnum_t : uint8_t{
    Circle,
    Rectangle
  };

  struct ShapeData_t{
    ShapeEnum_t ShapeEnum;

    /* TODO why we have ShapeID here? */
    ShapeID_t ShapeID;
  };

  #include "Shape/Circle/Types.h"
  #include "Shape/Rectangle/Types.h"

  #include "Object.h"

  struct ShapeInfoPack_t{
    ObjectID_t ObjectID;
    ShapeEnum_t ShapeEnum;
    ShapeID_t ShapeID;
  };

  #if ETC_BCOL_set_SupportGrid == 1
    #include "Grid.h"
  #endif

  ShapeList_Circle_t ShapeList_Circle;
  ShapeList_Rectangle_t ShapeList_Rectangle;

  #if ETC_BCOL_set_DynamicToDynamic == 1
    struct Contact_Shape_Flag{
      constexpr static uint32_t EnableContact = 0x01;
    };

    typedef void (*PreSolveAfter_Shape_cb_t)(
      __ETC_BCOL_P(t) *,
      const ShapeInfoPack_t *,
      const ShapeInfoPack_t *
    );

    struct Contact_Shape_t{
      uint32_t Flag = Contact_Shape_Flag::EnableContact;
      PreSolveAfter_Shape_cb_t AfterCB = [](
        __ETC_BCOL_P(t) *,
        const ShapeInfoPack_t *,
        const ShapeInfoPack_t *
      ){};
    };

    typedef void (*PreSolve_Shape_cb_t)(
      __ETC_BCOL_P(t) *,
      const ShapeInfoPack_t *,
      const ShapeInfoPack_t *,
      Contact_Shape_t *
    );

    void Contact_Shape_EnableContact(
      Contact_Shape_t *Contact
    ){
      Contact->Flag |= Contact_Shape_Flag::EnableContact;
    }
    void Contact_Shape_DisableContact
    (
      Contact_Shape_t *Contact
    ){
      Contact->Flag ^= Contact->Flag & Contact_Shape_Flag::EnableContact;
    }

    PreSolve_Shape_cb_t PreSolve_Shape_cb;
  #endif

  #if ETC_BCOL_set_SupportGrid == 1
    _f GridBlockSize;
    PreSolve_Grid_cb_t PreSolve_Grid_cb;
  #endif
  #ifdef ETC_BCOL_set_PostSolve_Grid
    PostSolve_Grid_cb_t PostSolve_Grid_cb;
  #endif

  #if ETC_BCOL_set_StepNumber == 1
    uint64_t StepNumber;
  #endif

  #include "../Collision/Collision.h"
  #include "../Object.h"
  #include "../BaseFunctions.h"
  #include "../Traverse.h"
  #include "../Shape/Shape.h"
  #include "../ObjectShape.h"
  #if ETC_BCOL_set_SupportGrid == 1
    #include "../Grid.h"
  #endif
  #include "../Step/Step.h"
  #include "../CompiledShapes.h"
  #include "../ImportHM.h"
};

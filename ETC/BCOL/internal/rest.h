#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(VEC/VEC.h)
#include _WITCH_PATH(MATH/MATH.h)

#define __ETC_BCOL_P(p0) CONCAT3(ETC_BCOL_set_prefix, _, p0)
#define __ETC_BCOL_PP(p0) CONCAT4(_, ETC_BCOL_set_prefix, _, p0)

#include _WITCH_PATH(ETC/BCOL/internal/Types/Types.h)

#undef __ETC_BCOL_PP
#undef __ETC_BCOL_P

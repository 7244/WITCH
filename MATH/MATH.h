#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#define MATH_constant_PI M_PI
#define MATH_constant_PI_D2 M_PI_2

static f32_t MATH_floor_f32(f32_t v){
  return floor(v);
}
static f64_t MATH_floor_f64(f64_t v){
  return floor(v);
}

static f32_t MATH_sin_32(f32_t v){
  return sin(v);
}
static f64_t MATH_sin_64(f64_t v){
  return sin(v);
}

static f32_t MATH_cos_32(f32_t v){
  return cos(v);
}
static f64_t MATH_cos_64(f64_t v){
  return cos(v);
}

static f32_t MATH_atan2_f32(f32_t p0, f32_t p1){
  return atan2(p0, p1);
}
static f64_t MATH_atan2_f64(f64_t p0, f64_t p1){
  return atan2(p0, p1);
}

static f32_t MATH_sqrt_32(f32_t v){
  return sqrt(v);
}
static f64_t MATH_sqrt_64(f64_t v){
  return sqrt(v);
}

static f32_t MATH_abs_f32(f32_t v){
  return fabs(v);
}
static f64_t MATH_abs_f64(f64_t v){
  return fabs(v);
}

static sint32_t MATH_abs_i32(sint32_t v){
  return abs(v);
}
static sint64_t MATH_abs_i64(sint64_t v){
  return abs(v);
}

static f32_t MATH_mod_f32(f32_t p0, f32_t p1){
  return fmod(p0, p1);
}
static f64_t MATH_mod_f64(f64_t p0, f64_t p1){
  return fmod(p0, p1);
}

static f32_t MATH_copysign_f32(f32_t p0, f32_t p1){
  return copysign(p0, p1);
}
static f64_t MATH_copysign_f64(f64_t p0, f64_t p1){
  return copysign(p0, p1);
}

static f32_t MATH_min_f32(f32_t p0, f32_t p1){
  return p0 < p1 ? p0 : p1;
}
static f64_t MATH_min_f64(f64_t p0, f64_t p1){
  return p0 < p1 ? p0 : p1;
}

static f32_t MATH_max_f32(f32_t p0, f32_t p1){
  return p0 > p1 ? p0 : p1;
}
static f64_t MATH_max_f64(f64_t p0, f64_t p1){
  return p0 > p1 ? p0 : p1;
}

static f32_t MATH_clamp_f32(f32_t p0, f32_t p1, f32_t p2){
  f32_t r = p0;
  r = r < p1 ? p1 : r;
  r = r > p2 ? p2 : r;
  return r;
}

static f32_t MATH_dot1_f32(f32_t p00, f32_t p10){
  return p00 * p10;
}
static f32_t MATH_dot2_f32(f32_t p00, f32_t p01, f32_t p10, f32_t p11){
  return p00 * p10 + p01 * p11;
}
static f32_t MATH_dot3_f32(f32_t p00, f32_t p01, f32_t p02, f32_t p10, f32_t p11, f32_t p12){
  return p00 * p10 + p01 * p11 + p02 * p12;
}

static void MATH_cross3_f32(
  f32_t p00,
  f32_t p01,
  f32_t p02,
  f32_t p10,
  f32_t p11,
  f32_t p12,
  f32_t *po0,
  f32_t *po1,
  f32_t *po2
){
  *po0 = p01 * p12 - p11 * p02;
  *po1 = p02 * p10 - p12 * p00;
  *po2 = p00 * p11 - p10 * p01;
}

static f32_t MATH_hypotenuse_f32(f32_t p0, f32_t p1){
  return MATH_sqrt_32(p0 * p0 + p1 * p1);
}

static bool MATH_Is_Point_Inside_Circle_f32(
  f32_t p0Y,
  f32_t p0X,
  f32_t p1Y,
  f32_t p1X,
  f32_t p1Size
){
  f32_t dY = p1Y - p0Y;
  f32_t dX = p1X - p0X;
  dY *= dY;
  dX *= dX;
  f32_t DistanceSquared = dY + dX;
  f32_t RadiusSquared = p1Size * p1Size;
  return DistanceSquared <= RadiusSquared;
}

static void MATH_NormalResolve_f32(
  f32_t DirectionY,
  f32_t DirectionX,
  f32_t NormalY,
  f32_t NormalX,
  f32_t Slideness,
  f32_t *oDirectionY,
  f32_t *oDirectionX
){
  f32_t kZ;
  f32_t kY;
  f32_t kX;
  MATH_cross3_f32(NormalX, NormalY, 0, 0, 0, -1, &kX, &kY, &kZ);
  /*
  fan::vec2 b = a - n * 2.f * MATH_dot2_f32(a.y, n.y, a.x, n.x);
  */
  f32_t Multipler = MATH_dot3_f32(kX, kY, kZ, DirectionX, DirectionY, 0);
  *oDirectionY = kY * Multipler;
  *oDirectionX = kX * Multipler;
}

static void MATH_CirclePackCircle(uint32_t Count, uint32_t Current, f32_t *r, f32_t *px, f32_t *py){
  *r = Count;
  *px = -(f32_t)(Count - 1) + Current * 2;
  *py = 0;
}

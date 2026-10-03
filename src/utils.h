#include "cglm/types.h"
#include "cglm/vec3.h"
#include "core.h"

f32 xz_angle(vec3 dir) {
  dir[1] = 0;
  glm_vec3_normalize(dir);
  return fmod(atan2(dir[2], dir[0]) + TAU, TAU);
}

f32 xz_dist(vec3 a, vec3 b) {
  return sqrt(pow(fabs(a[0] - b[0]), 2) + pow(fabs(a[2] - b[2]), 2));
}

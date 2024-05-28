#include <cmath>

#include "figure.h"

Quat getQuat(Vector* rotate_vector, float rotate_angle) {
  float angl_half = rotate_angle/2.0;
  return Quat{
	cos(angl_half),
	rotate_vector->x * sin(angl_half),
	rotate_vector->y * sin(angl_half),
	rotate_vector->z * sin(angl_half)
  };
}

Quat quatScale(Quat* q, float v) {
  return Quat{
	q->w * v,
	q->x * v,
	q->y * v,
	q->z * v
  };
}

float quatABS(Quat *q) {
  return sqrt(pow(q->w, 2) + pow(q->x, 2) + pow(q->y, 2) + pow(q->z, 2));
}

Quat quatNormlize(Quat *q) {
  float abs = quatABS(q);
  return quatScale(q, 1/abs);
}

Quat quatInvert(Quat* q) {
  Quat result{
	q->w,
	-1*q->x,
	-1*q->y,
	-1*q->z
  };
  return quatNormlize(&result);
}

Quat quatMulQuat(Quat* a, Quat* b) {
  return Quat{
	a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z,
	a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y,
	a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x,
	a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w
  };
}

Quat quatMulVector(Quat* a, Vector* b) {
  return Quat{
	-1*a->x*b->x - a->y*b->y - a->z*b->z,
	a->w*b->x + a->y*b->z - a->z*b->y,
	a->w*b->y - a->x*b->z + a->z*b->x,
	a->w*b->z + a->x*b->y - a->y*b->x
  };
}

Vector quatTransformVector(Quat* q, Vector* v) {
  Quat t = quatMulVector(q, v);
  Quat inverted_q = quatInvert(q);
  t = quatMulQuat(&t, &inverted_q);

  return Vector(t.x, t.y, t.z);
}

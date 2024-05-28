#include <cmath>

#include "figure.h"


Vector::Vector(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector Vector::VMultiply(Vector* c) {
  float x = 0, y = 0, z = 0;
  x = this->y * c->z - this->z * c->y;
  y = this->x * c->z - this->z * c->x;
  z = this->x * c->y - this->y * c->x;

  Vector newV(x, y, z);
  
  return newV;
}

float Vector::Scalar(Vector* c) {
  return this->x*c->x + this->y*c->y + this->z*c->z;
}

Vector Vector::Multiply(float v) {
  Vector newV(this->x*v, this->y*v, this->z*v);
  return newV;
}

Vector Vector::Divide(float v) {
  Vector newV(this->x/v, this->y/v, this->z/v);
  return newV;
}

Vector Vector::Plus(Vector *c) {
  Vector newV(this->x+c->x, this->y+c->y, this->z+c->z);
  return newV;
}

Vector Vector::Minus(Vector *c) {
  Vector newV(this->x-c->x, this->y-c->y, this->z-c->z);
  return newV;
}

float Vector::ABS() {
  return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

Vector Vector::Rotate(float angl, Vector* e) {
  Quat q = getQuat(e, angl);
 
  Vector r = quatTransformVector(&q, this);
  
  return r;
}

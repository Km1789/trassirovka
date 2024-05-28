#include "figure.h"

Sphere::Sphere(float x, float y, float z, float r) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->r = r;
}

Coord Sphere::Check(float x, float y, float z, Vector *r) {
  Vector normal(0,0,0);
  Vector V(0,0,0);
  Coord result{NAN, NAN, NAN, normal, V};
  
  float dx = x - this->x,
	dy = y - this->y,
	dz = z - this->z;

  float a = r->x*r->x + r->y*r->y + r->z*r->z,
	b = 2.0 * (dx * r->x + dy * r->y + dz * r->z),
	c = dx*dx + dy*dy + dz*dz - this->r*this->r;

  float D = b*b - 4*a*c;
  if (D < 0) {
  } else if (D == 0) {
	float t = -1*b/(2*a);
	float pX = r->x*t + x,
	  pY = r->y*t + y,
	  pZ = r->z*t + z;

	Vector fromSphere(x-pX, y-pY, z-pZ);
	if (fromSphere.Scalar(r) < 0) {
	  result.x = pX;
	  result.y = pY;
	  result.z = pZ;
	}
  } else {
	float t1 = (-1*b+sqrt(D))/(2*a),
	  t2 = (-1*b-sqrt(D))/(2*a);

	float pX1 = r->x*t1 + x,
	  pY1 = r->y*t1 + y,
	  pZ1 = r->z*t1 + z;

	Vector fromSphereT1(x-pX1, y-pY1, z-pZ1);
	float fromSphereScalarT1 = fromSphereT1.Scalar(r);

	float pX2 = r->x*t2 + x,
	  pY2 = r->y*t2 + y,
	  pZ2 = r->z*t2 + z;

	Vector fromSphereT2(x-pX2, y-pY2, z-pZ2);
	float fromSphereScalarT2 = fromSphereT2.Scalar(r);

	float D1 = fromSphereT1.ABS(),
	  D2 = fromSphereT2.ABS();

	if (fromSphereScalarT1 < 0 && fromSphereScalarT2 < 0) {
	  if (D1 < D2) {
		result.x = pX1;
		result.y = pY1;
		result.z = pZ1;
		result.V = fromSphereT1.Divide(D1);
	  } else {
		result.x = pX2;
		result.y = pY2;
		result.z = pZ2;
		result.V = fromSphereT2.Divide(D2);
	  }
	} else if (fromSphereScalarT1 < 0) {
	  result.x = pX1;
	  result.y = pY1;
	  result.z = pZ1;
	  result.V = fromSphereT1.Divide(D1);
	} else if (fromSphereScalarT2 < 0) {
	  result.x = pX2;
	  result.y = pY2;
	  result.z = pZ2;
	  result.V = fromSphereT2.Divide(D2);
	}
  }

  if (!isnan(result.x)) {
	Vector OX(result.x-this->x, result.y-this->y, result.z-this->z);
	float abs_ = OX.ABS();
	result.normal.x = OX.x / abs_;
	result.normal.y = OX.y / abs_;
	result.normal.z = OX.z / abs_;
  }

  return result;
}

Coord Sphere::MinimalD(float x, float y, float z) {
  Vector toCenter(this->x-x, this->y-y, this->z-z);
  toCenter = toCenter.Divide(toCenter.ABS());
  
  return this->Check(x, y, z, &toCenter);
}

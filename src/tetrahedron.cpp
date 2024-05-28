#include "figure.h"

void Tetrahedron::middlePoint() {
  float center1x = (this->x1+this->x2+this->x3)/3.0,
	center1y = (this->y1+this->y2+this->y3)/3.0,
	center1z = (this->z1+this->z2+this->z3)/3.0,

	center2x = (this->x1+this->x2+this->x4)/3.0,
	center2y = (this->y1+this->y2+this->y4)/3.0,
	center2z = (this->z1+this->z2+this->z4)/3.0;

  Vector v1(this->x4-center1x, this->y4-center1y, this->z4-center1z),
	v2(this->x3-center2x, this->y3-center2y, this->z3-center2z);
  
  Vector empty(0,0,0);

  v1 = v1.Divide(v1.ABS());
  v2 = v2.Divide(v2.ABS());

  float D = -1*(v1.x*center1x + v1.y*center1y + v1.z*center1z);

  float a = v1.Scalar(&v2);
  if (a == 0) {
	this->middleX = NAN;
	this->middleY = NAN;
	this->middleZ = NAN;
	
	return;
  }
  
  float b = v1.x*center2x + v1.y*center2y + v1.z*center2z + D;
  
  float t = -1*b/a;

  this->middleX = v2.x*t+center2x;
  this->middleY = v2.y*t+center2y;
  this->middleZ = v2.z*t+center2z;
}

TetrahedronCoords
Tetrahedron::GetCoords(float x1,
					   float y1,
					   float z1,
					   float x2,
					   float y2,
					   float z2,
					   float x3,
					   float y3,
					   float z3)
{
  float a1=x2-x1,
	a2 = x3-x1,
	b1 = y2-y1,
	b2 = y3-y1,
	c1 = z2-z1,
	c2 = z3-z1;

  float d1 = b1*c2-b2*c1,
	d2 = -1*(a1*c2-a2*c1),
	d3 = a1*b2-a2*b1;

  Vector normal(d1,d2,d3);
  normal = normal.Divide(normal.ABS());

  Vector toCenter(this->middleX - x1, this->middleY - y1, this->middleZ - z1);
  toCenter = toCenter.Divide(toCenter.ABS());
  if (toCenter.Scalar(&normal) > 0) {
	normal.x *= -1;
	normal.y *= -1;
	normal.z *= -1;
  }
  
  Vector empty(0,0,0);
  return TetrahedronCoords{x1,y1,z1,x2,y2,z2,x3,y3,z3,
						   y1*d2 - x1*d1 - z1*d3,
						   Coord{x1,y1,z1,normal.Divide(normal.ABS()), empty}};
}

Tetrahedron::Tetrahedron(float x1,
						 float y1,
						 float z1,
						 float x2,
						 float y2,
						 float z2,
						 float x3,
						 float y3,
						 float z3,
						 float x4,
						 float y4,
						 float z4)
{
  this->x1 = x1;
  this->y1 = y1;
  this->z1 = z1;
  this->x2 = x2;
  this->y2 = y2;
  this->z2 = z2;
  this->x3 = x3;
  this->y3 = y3;
  this->z3 = z3;
  this->x4 = x4;
  this->y4 = y4;
  this->z4 = z4;

  this->middlePoint();

  this->planes.push_back(this->GetCoords(x1,y1,z1,x2,y2,z2,x3,y3,z3));

  this->planes.push_back(this->GetCoords(x1,y1,z1,x2,y2,z2,x4,y4,z4));

  this->planes.push_back(this->GetCoords(x1,y1,z1,x3,y3,z3,x4,y4,z4));

  this->planes.push_back(this->GetCoords(x2,y2,z2,x3,y3,z3,x4,y4,z4));
}


bool
pointCheck(float x1,
		   float y1,
		   float z1,
		   float x2,
		   float y2,
		   float z2,
		   float x3,
		   float y3,
		   float z3)
{
  float max_x = x2 > x3 ? x2 : x3,
	min_x = x2 < x3 ? x2 : x3,
				 max_y = y2 > y3 ? y2 : y3,
	min_y = y2 < y3 ? y2 : y3,
				 max_z = z2 > z3 ? z2 : z3,
	min_z = z2 < z3 ? z2 : z3;

  if (x1 < min_x || x1 > max_x ||
	  y1 < min_y || y1 > max_y ||
	  z1 < min_z || z1 > max_z) {
	
	return false;
  }

  return true;
}


Coord Tetrahedron::Check(float x, float y, float z, Vector *r) {
  float a = 0.0, b = 0.0, t = 0.0;

  Vector empty(0, 0, 0);

  Coord coords[3] = {
	Coord{NAN, NAN, NAN, empty, empty},
	Coord{NAN, NAN, NAN, empty, empty},
	Coord{NAN, NAN, NAN, empty, empty}
  };
  int currentCoordInd = 0;

  for (int l=0; l<this->planes.size(); l++) {
	a = this->planes[l].coord.normal.Scalar(r);

	if (a == 0) {
	  continue;
	}
	
	b = this->planes[l].coord.normal.x*(this->planes[l].coord.x - x) +
	  this->planes[l].coord.normal.y*(this->planes[l].coord.y - y) +
	  this->planes[l].coord.normal.z*(this->planes[l].coord.z - z);

	t = b/a;

	float x_ = t*r->x + x,
	  y_ = t*r->y + y,
	  z_ = t*r->z + z;

	Vector P1(this->planes[l].x1-x_, this->planes[l].y1-y_, this->planes[l].z1-z_),
	  P2(this->planes[l].x2-x_, this->planes[l].y2-y_, this->planes[l].z2-z_),
	  P3(this->planes[l].x3-x_, this->planes[l].y3-y_, this->planes[l].z3-z_);

	float P1_ABS = P1.ABS(),
	  P2_ABS = P2.ABS(),
	  P3_ABS = P3.ABS();

	float cos_P1_P2 = P1.Scalar(&P2) / (P1_ABS * P2_ABS),
	  cos_P1_P3 = P1.Scalar(&P3) / (P1_ABS * P3_ABS),
	  cos_P2_P3 = P2.Scalar(&P3) / (P2_ABS * P3_ABS);

	float anglSum = 0.0;

	bool flag = false;
	
	if (cos_P1_P2 >= -1 && cos_P1_P2 <= 1) {
	  anglSum += acos(cos_P1_P2);
	} else if (pointCheck(x_, y_, z_,
						  this->planes[l].x1, this->planes[l].y1, this->planes[l].z1,
						  this->planes[l].x2, this->planes[l].y2, this->planes[l].z2)) {
	  flag = true;
	}

	if (cos_P1_P3 >= -1 && cos_P1_P3 <= 1) {
	  anglSum += acos(cos_P1_P3);
	} else if (pointCheck(x_, y_, z_,
						  this->planes[l].x1, this->planes[l].y1, this->planes[l].z1,
						  this->planes[l].x3, this->planes[l].y3, this->planes[l].z3)) {
	  flag = true;
	}

	if (cos_P2_P3 >= -1 && cos_P2_P3 <= 1) {
	  anglSum += acos(cos_P2_P3);	  
	} else if (pointCheck(x_, y_, z_,
						  this->planes[l].x2, this->planes[l].y2, this->planes[l].z2,
						  this->planes[l].x3, this->planes[l].y3, this->planes[l].z3)) {
	  flag = true;
	}

	anglSum = anglSum*180/M_PI;

	if (anglSum < 359.999 && !flag) {
	  continue;
	}

	Vector V(x-x_, y-y_, z-z_);

	Coord coord{x_, y_, z_, this->planes[l].coord.normal, V.Divide(V.ABS())};
	coords[currentCoordInd] = coord;

	currentCoordInd ++;
	if (currentCoordInd == 3) {
	  break;
	}
  }

  float d_min = 10000000000;
  int d_min_ind = -1;
  
  for (int i=0; i<currentCoordInd; i++) {
	float D = sqrt(pow(coords[i].x-x, 2) + pow(coords[i].y-y,2) + pow(coords[i].z-z, 2)),
	  scalar = coords[i].V.Scalar(r);

	if (scalar > 0) {
	  continue;
	}

	if (D < d_min) {
	  d_min = D;
	  d_min_ind = i;
	}
  }

  if (d_min_ind == -1) {
	return Coord{NAN, NAN, NAN, empty, empty};
  }
  
  return coords[d_min_ind];
}

Coord Tetrahedron::MinimalD(float x, float y, float z) {
  Vector toCenter(this->middleX-x, this->middleY-y, this->middleZ-z);
  toCenter = toCenter.Divide(toCenter.ABS());
  
  return this->Check(x, y, z, &toCenter);
}

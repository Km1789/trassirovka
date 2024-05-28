#include "figure.h"

Box::Box(float x1, float y1, float z1, float x2, float y2, float z2) {
  this->x1 = x1;
  this->y1 = y1;
  this->z1 = z1;
  this->x2 = x2;
  this->y2 = y2;
  this->z2 = z2;

  Vector i(1, 0, 0), j(0, 1, 0), k(0, 0, 1),
	i_(-1, 0, 0), j_(0, -1, 0), k_(0, 0, -1),
	empty(0, 0, 0);

  Coord XOZ{x1, y1, z1, j_, empty};
  this->planes.push_back(XOZ);

  Coord YOZ{x1, y1, z1, i_, empty};
  this->planes.push_back(YOZ);

  Coord XOY{x1, y1, z1, k_, empty};
  this->planes.push_back(XOY);

  Coord XOZ_{x2, y2, z2, j, empty};
  this->planes.push_back(XOZ_);

  Coord YOZ_{x2, y2, z2, i, empty};
  this->planes.push_back(YOZ_);

  Coord XOY_{x2, y2, z2, k, empty};
  this->planes.push_back(XOY_);
}

Coord Box::Check(float x, float y, float z, Vector *r) {
  Vector empty(0, 0, 0);

  float a = 0.0, b = 0.0, t = 0.0;

  Coord coords[4] = {
	Coord{NAN, NAN, NAN, empty, empty},
	Coord{NAN, NAN, NAN, empty, empty},
	Coord{NAN, NAN, NAN, empty, empty},
	Coord{NAN, NAN, NAN, empty, empty},
  };
  int currentCoordInd = 0;

  for (int l=0; l<this->planes.size(); l++) {
	a = this->planes[l].normal.Scalar(r);

	if (a == 0) {
	  continue;
	}
	
	b = this->planes[l].normal.x*(this->planes[l].x - x) +
	    this->planes[l].normal.y*(this->planes[l].y - y) +
	    this->planes[l].normal.z*(this->planes[l].z - z);
	
	t = b/a;

	float x_ = t*r->x + x,
	  y_ = t*r->y + y,
	  z_ = t*r->z + z;

	if (x_ < this->x1-0.0001 || x_ > this->x2+0.0001 ||
		y_ < this->y1-0.0001 || y_ > this->y2+0.0001 ||
		z_ < this->z1-0.0001 || z_ > this->z2+0.0001) {
	  continue;
	}

	Vector V(x-x_, y-y_, z-z_);

	Coord coord{x_, y_, z_, this->planes[l].normal, V.Divide(V.ABS())};
	coords[currentCoordInd] = coord;

	currentCoordInd ++;
	if (currentCoordInd == 4) {
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

Coord Box::MinimalD(float x, float y, float z) {
  float centerX = this->x1+(this->x2-this->x1)/2.0,
	centerY = this->y1+(this->y2-this->y1)/2.0,
	centerZ = this->z1+(this->z2-this->z1)/2.0;

  Vector toCenter(centerX-x, centerY-y, centerZ-z);
  toCenter = toCenter.Divide(toCenter.ABS());
  
  return this->Check(x, y, z, &toCenter);
}

#include <vector>

#include "SDL2/SDL.h"

class Vector {
private:
public:
  float x,y,z;
  Vector(float x, float y, float z);
  Vector VMultiply(Vector *c);
  Vector Multiply(float v);
  Vector Divide(float v);
  Vector Plus(Vector *c);
  Vector Minus(Vector *c);
  Vector Rotate(float angl, Vector* e);
  float ABS();
  float Scalar(Vector* c);
};

struct ColorsSort {
  float D;
  int pos;
};

struct Quat {
  float w,x,y,z;
};

Quat getQuat(Vector* rotate_vector, float rotate_angle);
Vector quatTransformVector(Quat* q, Vector* v);

struct Coord {
  float x;
  float y;
  float z;
  Vector normal;
  Vector V;
};

class Figure {
public:
  virtual Coord Check(float x, float y, float z, Vector *r);
  virtual Coord MinimalD(float x, float y, float z);
};

class Camera {
private:
public:
  float x, y, z;
  Vector *normal, *up;
  float a0, a1, alpha;
  float w,h;
  float lightX, lightY, lightZ;
  Camera(float x, float y, float z,
		 Vector *normal, Vector *up,
		 float a0, float a1, float alpha,
		 float w, float h, float lx, float ly, float lz);
};

class Scene {
private:
  std::vector<Figure* > figures;
  Camera* camera;
  float width, height;

public:
  Scene(Camera* camera);
  void AddFigure(Figure* figure);

  void Show(SDL_Renderer* renderer);
};

class Sphere : public Figure {
private:
  float x,y,z,r;
public:
  Sphere(float x, float y, float z, float r);
  Coord Check(float x, float y, float z, Vector *r) override;
  Coord MinimalD(float x, float y, float z) override;
};

class Box : public Figure {
private:
  float x1,y1,z1,x2,y2,z2;
  std::vector<Coord> planes;
public:
  Box(float x1, float y1, float z1, float x2, float y2, float z2);
  Coord Check(float x, float y, float z, Vector *r) override;
  Coord MinimalD(float x, float y, float z) override;
};

struct TetrahedronCoords {
  float x1, y1, z1, x2, y2, z2, x3, y3, z3;
  float D;
  Coord coord;
};

class Tetrahedron : public Figure {
private:
  float x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4;
  std::vector<TetrahedronCoords> planes;
  float middleX, middleY, middleZ;
public:
  TetrahedronCoords GetCoords(float x1,
							  float y1,
							  float z1,
							  float x2,
							  float y2,
							  float z2,
							  float x3,
							  float y3,
							  float z3);

  Tetrahedron(float x1,
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
			  float z4);
  
  Coord Check(float x, float y, float z, Vector *r) override;
  void middlePoint();
  Coord MinimalD(float x, float y, float z) override;
};

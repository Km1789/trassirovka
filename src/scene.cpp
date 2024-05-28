#include <iostream>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <chrono>
#include <omp.h>

#include "omp.h"

#include "figure.h"


Camera::Camera(float x, float y, float z,
			   Vector *normal, Vector *up,
			   float a0, float a1, float alpha,
			   float w, float h, float lx, float ly, float lz)
{
  this->x = x;
  this->y = y;
  this->z = z;
  this->normal = normal;
  this->up = up;
  this->a0 = a0;
  this->a1 = a1;
  this->alpha = alpha;
  this->w = w;
  this->h = h;
  this->lightX = lx;
  this->lightY = ly;
  this->lightZ = lz;
}

Scene::Scene(Camera* camera) {
  this->camera = camera;
}

void Scene::AddFigure(Figure* figure) {
  this->figures.push_back(figure);
}

void Scene::Show(SDL_Renderer *renderer) {
  SDL_RenderClear(renderer);

  std::vector<float> colors;
  std::vector<ColorsSort> colorsSort;

  float min_gray = 50, newGray = 0.0;
  
  for (int figureInd=0; figureInd<this->figures.size(); figureInd++) {
	Coord c = this->figures[figureInd]->MinimalD(this->camera->x, this->camera->y, this->camera->z);

    colorsSort.push_back(ColorsSort{
		(float)sqrt(pow(c.x-this->camera->x, 2) + pow(c.y-this->camera->y, 2) + pow(c.z-this->camera->z, 2)),
		figureInd
	  });
	
	colors.push_back(min_gray);
  }
  
  for (int i=0; i<colorsSort.size()-1; i++) {
	for (int j=i+1; j<colorsSort.size(); j++) {
	  if (colorsSort[i].D > colorsSort[j].D) {
		ColorsSort cs_ = colorsSort[i];
		colorsSort[i] = colorsSort[j];
		colorsSort[j] = cs_;
	  }
	}
  }

  if (colorsSort.size() == 1) {
	colors[0] = 64;
  } else {
	float grayMax = 191,
	  grayMin = 64;

	float dMin = colorsSort[0].D;
	float dDelta = colorsSort[colorsSort.size()-1].D - dMin;
	float grayKf = 0.0;
	if (dDelta > 0) {
	  grayKf = (grayMax-grayMin)/(dDelta);
	}

	for (int i=0; i<colorsSort.size(); i++) {
	  colors[colorsSort[i].pos] = (colorsSort[i].D-dMin)*grayKf + grayMin;
	}
  }

  float	kh = this->camera->h / (2.0 * this->camera->a0 * tan(this->camera->alpha/2.0));
  float middle_x = (float)this->camera->w/kh/2.0,
	middle_y = (float)this->camera->h/kh/2.0;

  float half_pi = M_PI/2.0;

  int figures_size = this->figures.size();


  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now(); 


	 float i = 0, j = 0;

	 float dx = 0, dy = 0;

	 float anglABCx = 0;

	// Vector normalAnglABCxRotated = this->camera->normal->Multiply(this->camera->a0)
	// 	.Rotate(anglABCx, this->camera->up);
	  
	// Vector upPerpendicular = this->camera->up->Rotate(M_PI/2.0, this->camera->normal);

	// float anglABCy = 0;
	// Vector normalAnglABCyRotated = normalAnglABCxRotated.Rotate(anglABCy, &upPerpendicular);

	// float screenX = normalAnglABCyRotated.x + this->camera->x,
	// screenY = normalAnglABCyRotated.y + this->camera->y,
	// screenZ = normalAnglABCyRotated.z + this->camera->z;

	// Vector P = normalAnglABCyRotated.Divide(normalAnglABCyRotated.ABS());	  

  Vector fromTo(0,0,0);
  float currentD = fromTo.ABS();
  bool flag = false;
  float min_d = 1000000000000;
  newGray = 0.0;
  #pragma omp parallel for private(newGray) num_threads(4)
  for (int i_=0; i_ < (int)this->camera->w; i_++) {
	for (int j_=0; j_ < (int)this->camera->h; j_++) {
	  float i = i_/kh, j = j_/kh;

	  float dx = i-middle_x,
		dy = j-middle_y;

	  float anglABCx = atan(-1*dx/this->camera->a0);

	  Vector normalAnglABCxRotated = this->camera->normal->Multiply(this->camera->a0)
		.Rotate(anglABCx, this->camera->up);
	  
	  Vector upPerpendicular = this->camera->up->Rotate(M_PI/2.0, this->camera->normal);

	  float anglABCy = atan(-1*dy/normalAnglABCxRotated.ABS());
	  Vector normalAnglABCyRotated = normalAnglABCxRotated.Rotate(anglABCy, &upPerpendicular);

	  float screenX = normalAnglABCyRotated.x + this->camera->x,
		screenY = normalAnglABCyRotated.y + this->camera->y,
		screenZ = normalAnglABCyRotated.z + this->camera->z;

	  Vector P = normalAnglABCyRotated.Divide(normalAnglABCyRotated.ABS());	  

	  float min_d = 1000000000000;
	  bool flag = false;
      //#pragma omp critical
	  {
		for (int figureInd = 0; figureInd < figures_size; figureInd++) {
			
		  Coord point = this->figures[figureInd]->Check(screenX, screenY, screenZ, &P);
		  
		  if (!isnan(point.x)) {
			flag = true;
		  
			Vector fromTo(point.x-screenX, point.y-screenY, point.z-screenZ);
			float currentD = fromTo.ABS();
		  
			if (min_d > currentD) {
			  min_d = currentD;
			  newGray = 0.0;

			  Vector toLight(this->camera->lightX-point.x,
							 this->camera->lightY-point.y,
							 this->camera->lightZ-point.z);
			  Vector fromLight(point.x - this->camera->lightX,
							   point.y - this->camera->lightY,
							   point.z - this->camera->lightZ);

			  Vector nlScalar = point.normal.Multiply(2.0*point.normal.Scalar(&fromLight));
			  Vector r = fromLight.Minus(&nlScalar);

			  float anglVR = acos(point.V.Scalar(&r)/(point.V.ABS() * r.ABS()));

			  float Is = 255*(1-anglVR/half_pi);

			  toLight = toLight.Divide(toLight.ABS());

			  float Id = toLight.Scalar(&(point.normal));

			  if (Id >= 0) {
				newGray = colors[figureInd] * Id;
				if (Is >= 0) {
				  newGray = Is > newGray ? Is : newGray;
				}
			  }

			  newGray = min_gray > newGray ? min_gray : newGray;
			}
		  }
		  #pragma omp critical
		  {
			if (flag && min_d <= camera->a1) {
			  SDL_SetRenderDrawColor(renderer, newGray, newGray, newGray, 255);
			} else {
			  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
			SDL_RenderDrawPoint(renderer, i_, j_);
		  }
		}
	  }
	}
  }
std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now(); 
int elapsed_ms = static_cast<int>( std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() ); 
std::cout << "Addition operator runtime is "<< elapsed_ms << " ms\n";

  std::cout << "Start rendering\n";

  SDL_RenderPresent(renderer);
}

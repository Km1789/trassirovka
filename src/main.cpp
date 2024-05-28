#include <iostream>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <chrono>
#include <string>
#include <time.h>

#include "figure.h"

bool saveScreenshotBMP(std::string filepath, SDL_Window* SDLWindow, SDL_Renderer* SDLRenderer) {
    SDL_Surface* saveSurface = NULL;
    SDL_Surface* infoSurface = NULL;
    infoSurface = SDL_GetWindowSurface(SDLWindow);
    if (infoSurface == NULL) {
        std::cerr << "Failed to create info surface from window in saveScreenshotBMP(string), SDL_GetError() - " << SDL_GetError() << "\n";
    } else {
        unsigned char * pixels = new (std::nothrow) unsigned char[infoSurface->w * infoSurface->h * infoSurface->format->BytesPerPixel];
        if (pixels == 0) {
            std::cerr << "Unable to allocate memory for screenshot pixel data buffer!\n";
            return false;
        } else {
            if (SDL_RenderReadPixels(SDLRenderer, &infoSurface->clip_rect, infoSurface->format->format, pixels, infoSurface->w * infoSurface->format->BytesPerPixel) != 0) {
                std::cerr << "Failed to read pixel data from SDL_Renderer object. SDL_GetError() - " << SDL_GetError() << "\n";
                delete[] pixels;
                return false;
            } else {
                saveSurface = SDL_CreateRGBSurfaceFrom(pixels, infoSurface->w, infoSurface->h, infoSurface->format->BitsPerPixel, infoSurface->w * infoSurface->format->BytesPerPixel, infoSurface->format->Rmask, infoSurface->format->Gmask, infoSurface->format->Bmask, infoSurface->format->Amask);
                if (saveSurface == NULL) {
                    std::cerr << "Couldn't create SDL_Surface from renderer pixel data. SDL_GetError() - " << SDL_GetError() << "\n";
                    delete[] pixels;
                    return false;
                }
                SDL_SaveBMP(saveSurface, filepath.c_str());
                SDL_FreeSurface(saveSurface);
                saveSurface = NULL;
            }
            delete[] pixels;
        }
        SDL_FreeSurface(infoSurface);
        infoSurface = NULL;
    }
    return true;
}


void show(Camera* camera, SDL_Renderer *renderer) {
  Scene scene(camera);

  std::string some;

  float x1=0,y1=0,z1=0,
	x2=0,y2=0,z2=0,
	x3=0,y3=0,z3=0,
	x4=0,y4=0,z4=0;

  std::vector<Sphere> spheres;
  std::vector<Box> boxes;
  std::vector<Tetrahedron> ttrhdrn;

  std::ifstream infig("./figures.txt");
  while(infig >> some) {
	if (some == "sphere") {
	  infig >> x1 >> y1 >> z1 >> x2;
	  Sphere sphere(x1,y1,z1,x2);
	  spheres.push_back(sphere);
	} else if (some == "box") {
	  infig >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
	  Box box(x1, y1, z1, x2, y2, z2);
	  boxes.push_back(box);
	} else if (some == "tetra") {
	  infig >> x1 >> y1 >> z1 >> x2 >> y2 >> z2
			>> x3 >> y3 >> z3 >> x4 >> y4 >> z4;
	  Tetrahedron tetrahedron(x1, y1, z1,
							  x2, y2, z2,
							  x3, y3, z3,
							  x4, y4, z4);
	  ttrhdrn.push_back(tetrahedron);
	}
  }

  for (int i=0; i<spheres.size(); i++) {
	scene.AddFigure(&(spheres[i]));
  }
  for (int i=0; i<boxes.size(); i++) {
	scene.AddFigure(&(boxes[i]));
  }
  for (int i=0; i<ttrhdrn.size(); i++) {
	scene.AddFigure(&(ttrhdrn[i]));
  }

  infig.close();

  scene.Show(renderer);
}

int screen_show() {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
	fprintf(stderr, "Could not init SDL: %s\n", SDL_GetError());
	return 1;
  }

  std::string some;
  float camX = 0, camY = 0, camZ = 0,
	nX = 0, nY = 0, nZ = 0,
	upX = 0, upY = 0, upZ = 0,
	a0 = 0, limit = 0, alpha = 0,
	w = 0, h = 0,
	lightX = 0, lightY = 0, lightZ = 0;

  std::ifstream in("./settings.txt");

  while (in >> some) {
	if (some == "cam") {
	  in >> camX >> camY >> camZ;
	} else if (some == "normal") {
	  in >> nX >> nY >> nZ;
	} else if (some == "up") {
	  in >> upX >> upY >> upZ;
	} else if (some == "screen") {
	  in >> a0;
	} else if (some == "limit") {
	  in >> limit;
	} else if (some == "alpha") {
	  in >> alpha;
	} else if (some == "width") {
	  in >> w;
	} else if (some == "height") {
	  in >> h;
	} else if (some == "light") {
	  in >> lightX >> lightY >> lightZ;
	}
  }
  
  in.close();

  Vector normal(nX,nY,nZ), up(upX,upY,upZ);
  
  Camera camera(camX, camY, camZ,
				&normal, &up, a0, limit, alpha*M_PI/180.0,
				w, h,
			    lightX, lightY, lightZ);

  SDL_Window *screen = SDL_CreateWindow("Simple 3D modeling",
										SDL_WINDOWPOS_UNDEFINED,
										SDL_WINDOWPOS_UNDEFINED,
										camera.w, camera.h,
										0);
  
  if(!screen) {
	fprintf(stderr, "Could not create window\n");
	return 1;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
  if(!renderer) {
	fprintf(stderr, "Could not create renderer\n");
	return 1;
  }

  show(&camera, renderer);

  saveScreenshotBMP("./result.bmp", screen, renderer);

  SDL_Event e;
  bool quit = false;
  while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }

  SDL_DestroyWindow(screen);
  SDL_Quit();

  return 0;
}

int main()
{
  screen_show();

  return 0;
}

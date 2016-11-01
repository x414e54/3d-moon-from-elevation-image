/*
I downloaded a simple grayscale heighmap from nasa, //http://wms.lroc.asu.edu/lroc/global_product/128_ppd_DEM
and then imported it into the previous question. This created a flat plane that looks somewhat moonlike.
There is a choice of different heightmaps with varying resolutions the one I chose was the 1Mb 
and was 4 pixels per degree. This is a rectangular based heightmap with the u and v around the equator.
We can use the approximate circumference of the moon  6,784 miles (10,917 km) and mean radius 1,737.10 km
to calculate the distance per pixel. We can then use a running speed of around 8 mph and head height of about 2 meters
to create a simulation of a person walking on the moon. Collision physics and gravity could be included also, but
to start with we will just put the view at 2 meters above the terrain height.

The next stage would be to create a calculation for a spherical mesh based on the heightmap and then
stream only the part of the mesh we currently need to render. I thought about creating the mesh and then
using some spacial database to store it and then stream from that. It might be posible to create a tri-tree for each triangle
face on an icosophere.

I decided to create a uv sphere and cut the algorithm to only the section we could visually see.
As the user then walks around, the heights of the vertecies in the dome can then be adjusted.
As the nasa heightmap is essentialy a uv texture we can use the equation below to pick the correct u and v 
from the image and then displace the vertex the correct amount. Currently there is a bug which means every two
segments there is a small jerk as the rotation is incorrect but I did not have time to fix it.

d = unit vector from origin to p.

u= 0.5 + (arctan2(d.z, d.x) / (2*PI));
v= 0.5 - (2.0 * (arcsin(d.y) / (2*PI)));

We also need to work out the difference in heights we can do this approximately as bellow:

Image is 1440 pixels at the equator in width, this gives us a radius of 1440/2PI pixels.
1,738.14 km is the radius of the moon and the moon is about 7km high and low.
(1440/2PI)/1738.14 gives us the pixel to km ratio
(1440/2PI)/1738.14x7 gives us the height range we will use;

Values should be converted from pixels to real meters.

This is just a prototype.

*/

#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include <sstream>

#include "geometry.h"
#include "opengl_renderer.hpp"

//http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

float MAXHEIGHTRANGE;

HeightMap heightMap;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Please specify location of nasa moon map" << std::endl;
		return 1;
	}

	// Init SDL and opengl
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	    return 1;
	}

    // Load all of heightmap into memory for now.
	// TODO Possibly convert this to streaming later.
	{
#if __APPLE__
        char* base_path = SDL_GetBasePath();
		std::stringstream temp_file;
		temp_file << base_path;
		temp_file << "temp.bmp";
		std::string test = temp_file.str();
        SDL_Surface *heightsimg = SDL_LoadBMP(test.c_str());
		SDL_free(base_path);
#else
        SDL_Surface *heightsimg = SDL_LoadBMP(argv[1]);//"WAC_GLD100_E000N1800_004P.TIF"); //http://wms.lroc.asu.edu/lroc/global_product/128_ppd_DEM
#endif
		if (!heightsimg) {
			std::cout << "could not load image " << argv[1] << std::endl;
    	    SDL_Quit();
    	    return 1;
		}

		heightMap.h=heightsimg->h;
		heightMap.w=heightsimg->w;
		MAXHEIGHTRANGE=7; // 7km from peak to circumference
		if (argc>2) { MAXHEIGHTRANGE = atoi(argv[2]); }
		MAXHEIGHTRANGE= ((heightMap.w / (2*PI)) / 1738.14) *MAXHEIGHTRANGE;
		heightMap.heights = new float*[heightMap.h];  
		for (int i = 0; i < heightMap.h; i++) {
		    heightMap.heights[i] = new float[heightMap.w];  
	    }
		
		for (int x=0; x <heightMap.w; x++) {
			for (int y=0; y <heightMap.h; y++) {
				Uint8 r, g, b;
				SDL_GetRGB(getpixel(heightsimg,x,y), heightsimg->format, &r, &g, &b);
				heightMap.heights[y][x] = getHeightFromColor(r,g,b, MAXHEIGHTRANGE);
			}
		}
		SDL_FreeSurface(heightsimg);
	}
	
	Renderer* renderer = new OpenGLRenderer();

	int pixelsperdegree=heightMap.w/360;
	int anglearea=20;
	int totalslices=heightMap.w;
	int totalstacks=heightMap.h;
	int radius=heightMap.w / (2*PI);
	float segmentoffset = (anglearea*pixelsperdegree);

	std::vector<Vertex*> list;
	createSphereDome(&list, radius, totalslices, totalstacks, anglearea);
	
	// TODO Convert this to vertex buffer
	int list1 = renderer->createList(list, 0.0f, radius);
	int list2 = renderer->createList(list, segmentoffset, radius);

	SDL_Event event;
	PlayerPosition pos;	
	pos.speed = 14.0f*(MAXHEIGHTRANGE/7000.0f); //7km/s
	pos.viewheight = 2.0f / MAXHEIGHTRANGE / 1000.0f;
	pos.radius = findHeight(heightMap, list[0]->v.getX(),list[0]->v.getY(),list[0]->v.getZ(), 0.0f);
	renderer->render(list1, pos, anglearea, pixelsperdegree);

	float lastTime = SDL_GetTicks();
	bool toggle = false;
	int segment = 1;
	bool running = true;
	while(running) {
		float time = SDL_GetTicks();
		float timeDelta = time - lastTime;
		if(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT: running = false;
				/*case SDL_MOUSEMOTION:
			  	//	  break;
				case SDL_KEYDOWN:
			  		switch(event.key.keysym.sym)
			  		{
			  			case SDLK_LEFT: break;
			  			case SDLK_UP: break;
			  			case SDLK_DOWN:  break;
			  			case SDLK_RIGHT: break;
						default: break;
			  		}
					break;*/
			}
		}
		pos.update(timeDelta);

		float location = fmodf(pos.azimuth,anglearea*2.0f);

		if (!toggle && location > anglearea) {
			segment++;
			toggle = true;
			renderer->updateList(list, list1, segment*segmentoffset, radius);
		} else if (toggle && location <= anglearea) {
			segment++;
			toggle = false;
			renderer->updateList(list, list2, segment*segmentoffset, radius);
		}

		renderer->render(list1, pos, anglearea, pixelsperdegree);
		lastTime=time;
	}

	//Cleanup here
	delete renderer;
	SDL_Quit();
}





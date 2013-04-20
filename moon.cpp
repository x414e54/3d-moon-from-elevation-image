#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "geometry.h"
#include <vector>

static double PI = 3.1415926535897932384626433832795;

struct HeightMap
{
	float** heights;
	int w, h;
} ;

struct PlayerPosition
{
	float inclination;	
	float azimuth;
	float radius;
	float viewheight;
	float orientation;
	float speed;
	void update(float timeDelta)
	{
		azimuth+=speed*(timeDelta);
	}
};

struct Vertex
{
	Vector3 v;
	Vector3 n;
	Vertex(float x, float y, float z, float nx, float ny, float nz) : v(x,y,z), n(nx,ny,nz) {}
};

//Finds the height from a uv heightmap
float findHeight(const HeightMap &heightmap, float x, float y, float z)
{
	int h=heightmap.h-1;
	int w=heightmap.w-1;
	// Calculate u v
	Vector3 d = Vector3(x,y,z);
	Vector3::Normalize(d);

	float u = 0.5 + (atan2(d.getZ(), d.getX()) / (2*PI));
	float v = 0.5 - (2.0 * (asin(d.getY()) / (2*PI)));	
	// Calculate array position on image;

	if (isnan(u) || isnan(v)) return 0;    // some uv are comming back nan this needs to be fixed, such as removing these vertices 
	int ix = (int)round(u*w);
	int iy = (int)round(v*h);
	return heightmap.heights[iy][ix];
}

inline void setColor(float height, float max)
{
	float c = ((height + max)) / (max*2.0f);
	glColor3f(c,c,c);		
}

inline float getHeightFromColor(Uint8 r, Uint8 g, Uint8 b, float max)
{
	return (max*r*2.0f/255.0f) - max;
}

inline void render(GLuint index, const PlayerPosition& pos)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//pos.radius + pos.viewheight set camera position to be just above terrain
	gluLookAt(1.0f, 300.0f , 0.0f,
			  0.0f, 0.0f, 0.0f,			  
	          0.0f, 1.0f, 0.0f);
	glRotatef(pos.azimuth, 1.0f, 1.0f, 1.0f);
	glCallList(index);
	SDL_GL_SwapBuffers();
}

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


/*


Adaptation of freeglut solidsphere for this example

*/

/*
 * freeglut_geometry.c
 *
 * Freeglut geometry rendering methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Compute lookup table of cos and sin values forming a cirle
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */

static void fghCircleTable(double **sint,double **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*M_PI/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        //fgError("Failed to allocate memory in fghCircleTable");
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}


/*
 * Draws a solid sphere
 */
void createSphereDome(std::vector<Vertex*>* list, GLdouble radius, GLint slices, GLint stacks, GLint anglearea)
{
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

	int stackrange = stacks/360*anglearea;
	int centerstack = stacks/2;
    for( i=1; i<stacks-1; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        //glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
				list->push_back(new Vertex(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius, cost1[j]*r1, sint1[j]*r1, z1));
				list->push_back(new Vertex(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius, cost1[j]*r0, sint1[j]*r0, z0));
               // glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                //glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                //glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                //glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            }

        //glEnd();
    }

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

float MAXHEIGHTRANGE;

HeightMap heightMap;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Please specify location of nasa moon map" << std::endl;
		return 1;
	}

	// Init SDL and opengl
	if(SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		std::cout << "SDL init" << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,            32);
 
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,        8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,        8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_OPENGL);
	if (!screen)
    	{
		std::cout << "could set video mode" << std::endl;
        SDL_Quit();
        return 1;
    	}

	SDL_WM_SetCaption("Walking on the Moon","O");
	SDL_EnableKeyRepeat(100, 100);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (float)800 / (float)600, 0.00001f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5.0f, 5.0f, 10.0f,
			  0.0f, 0.0f, 0.0f,
	          	    0.0, 1.0, 0.0);

	GLuint index = glGenLists(1);

	{
		SDL_Surface *heightsimg = IMG_Load(argv[1]);//"WAC_GLD100_E000N1800_004P.TIF"); //http://wms.lroc.asu.edu/lroc/global_product/128_ppd_DEM
		if (!heightsimg)
    		{
			std::cout << "could not load image " << argv[1] << std::endl;
    	    SDL_Quit();
    	    return 1;
    		}

		heightMap.h=heightsimg->h;
		heightMap.w=heightsimg->w;
		MAXHEIGHTRANGE= ((heightMap.w / (2*PI)) / 1738.14) *7*10;
		heightMap.heights = new float*[heightMap.h];  
		for (int i = 0; i < heightMap.h; i++)   
		    heightMap.heights[i] = new float[heightMap.w];  
	
		for (int x=0; x <heightMap.w; x++)
		{
			for (int y=0; y <heightMap.h; y++)
			{
				Uint8 r, g, b;
				SDL_GetRGB(getpixel(heightsimg,x,y), heightsimg->format, &r, &g, &b);
				heightMap.heights[y][x] = getHeightFromColor(r,g,b, MAXHEIGHTRANGE);
			}
		}
		SDL_FreeSurface(heightsimg);
	}	

	int pixelsperdegree=heightMap.w/360;
	int anglearea=10;
	int totalslices=heightMap.w;
	int totalstacks=heightMap.h;
	int radius=heightMap.w / (2*PI);

	std::vector<Vertex*> list;
	createSphereDome(&list, radius, totalslices, totalstacks, anglearea);

	// Inital list, as we get futher to the edge we must recreate the new list
	glNewList(index, GL_COMPILE);
		glBegin(GL_QUAD_STRIP);
		
		for(std::vector<Vertex*>::iterator vertex = list.begin(); vertex != list.end(); ++vertex) {
				float height =findHeight(heightMap, (*vertex)->v.getX(),(*vertex)->v.getY(),(*vertex)->v.getZ());
				setColor(height, MAXHEIGHTRANGE);
				Vector3::Normalize((*vertex)->v);
				(*vertex)->v*=radius+height;
                glNormal3f((*vertex)->n.getX(),(*vertex)->n.getY(),(*vertex)->n.getZ());
                glVertex3f((*vertex)->v.getX(),(*vertex)->v.getY(),(*vertex)->v.getZ());
		}

        glEnd();
	glEndList();

	SDL_Event event;
	float x=heightMap.w/2.0f;
	float y=heightMap.h/2.0f;
	float rot=0.0f;
	PlayerPosition pos;	
	pos.speed = 7.0f*(MAXHEIGHTRANGE/7000.0f); //7km/s
	pos.viewheight = 2.0f / MAXHEIGHTRANGE / 1000.0f;
	pos.radius = findHeight(heightMap, list[0]->v.getX(),list[0]->v.getY(),list[0]->v.getZ());
	render(index, pos);
	bool down = false;

	float lastTime = SDL_GetTicks();
	while(true)
	{
		float time = SDL_GetTicks();
		float timeDelta = time - lastTime;
		if(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT: SDL_Quit(); return 0; break;
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
		render(index,pos);
		lastTime=time;
	}

	//Cleanup here
}



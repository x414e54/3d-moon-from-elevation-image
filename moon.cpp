#include <iostream>
#include <SDL2/SDL.h>
#if __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#include "geometry.h"
#include <vector>

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
	createSphereDome(&list, radius, totalslices, totalstacks);

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



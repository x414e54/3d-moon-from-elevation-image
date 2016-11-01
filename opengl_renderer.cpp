//
//  opengl_renderer.cpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "opengl_renderer.hpp"

// This will be the only place gl.h is mentioned.
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// TODO remove glu
#if __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

struct OpenGLRendererImpl
{
    SDL_GLContext context;
    SDL_Window* window;
};

// TODO All window creation per renderer for now, should be a separate abstraction.
OpenGLRenderer::OpenGLRenderer()
{
    this->impl = new OpenGLRendererImpl();
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
 
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    impl->window = SDL_CreateWindow("Walking on the Moon",
                       SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED,
                       640, 480,
                       SDL_WINDOW_OPENGL);
	if (!impl->window) {
	    // TODO Add some way to fail.
	    return;
    }
	
	impl->context = SDL_GL_CreateContext(impl->window);
	if (!impl->context) {
	    // TODO Add some way to fail.
	    return;
    }
	
    SDL_GL_MakeCurrent(impl->window, impl->context);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (float)800 / (float)600, 0.00001f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5.0f, 5.0f, 10.0f,
			  0.0f, 0.0f, 0.0f,
	          	    0.0, 1.0, 0.0);
}

OpenGLRenderer::~OpenGLRenderer()
{
    SDL_GL_MakeCurrent(0, 0);
	SDL_GL_DeleteContext(impl->context);
	SDL_DestroyWindow(impl->window);
	delete this->impl;
}

const char* OpenGLRenderer::type_name = "OpenGL";

const char* OpenGLRenderer::get_name()
{
    return OpenGLRenderer::type_name;
}

int OpenGLRenderer::createList(std::vector<Vertex*> &list, float offset, int radius)
{
	GLuint index = glGenLists(1);
	updateList(list, index, offset, radius);
	return index;
}

inline void setColor(float height, float max)
{
	float c = ((height + max)) / (max*2.0f);
	glColor3f(c,c,c);
}

void OpenGLRenderer::updateList(std::vector<Vertex*> &list, int index, float offset, int radius)
{
	glNewList(index, GL_COMPILE);
		glBegin(GL_QUAD_STRIP);
	
			for(std::vector<Vertex*>::iterator vertex = list.begin(); vertex != list.end(); ++vertex)
			{
			    // TODO - this should be moved out and abstracted at a higher level.
				float height = findHeight(heightMap, (*vertex)->v.getX(),(*vertex)->v.getY(),(*vertex)->v.getZ(),  offset);
				setColor(height, MAXHEIGHTRANGE);
				Vector3::Normalize((*vertex)->v);
				(*vertex)->v*=radius+height;
        	    glVertex3f((*vertex)->v.getX(),(*vertex)->v.getY(),(*vertex)->v.getZ());
			}

        glEnd();
	glEndList();
}

void OpenGLRenderer::render(int index, const PlayerPosition& pos, int anglearea, int pixelsperdegree)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();//pos.radius + pos.viewheight set camera position to be just above terrain
	gluLookAt(1.0f, 231.5f , 0.0f,
			  0.0f, 231.0f, 0.0f,			  
	         0.0f, 1.0f, 0.0f);
	
	float location = fmodf(pos.azimuth,anglearea*2.0f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);			// rotate the model so we are above the dome
	glRotatef(-90.0f,1.0f,0.0f,0.0f);			// rotate the model so we are above the dome
	glRotatef(location, 0.0f, 1.0f, 0.0f);		// rotate only between 0 and 40
	float segmentrotation = -(anglearea - 0.25); // change 0.25 to match 1/pixels per degree
	if (location <= anglearea)						// which dome are we on?
	{
		glCallList(index);
		glRotatef(segmentrotation,0.0f,1.0f,0.0f);
		glCallList(index+1);
	} else {
		glRotatef(segmentrotation,0.0f,1.0f,0.0f);
		glCallList(index+1);
		glRotatef(segmentrotation,0.0f,1.0f,0.0f);
		glCallList(index);
	}
	SDL_GL_SwapWindow(impl->window);
}

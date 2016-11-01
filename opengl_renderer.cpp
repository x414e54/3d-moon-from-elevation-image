//
//  opengl_renderer.cpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "opengl_renderer.hpp"

// This will be the only place gl.h is mentioned.
#if __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#endif

const char* OpenGLRenderer::type_name = "OpenGL";

const char* OpenGLRenderer::get_name()
{
    return OpenGLRenderer::type_name;
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
	
	float location = 0.0f;//fmodf(pos.azimuth,anglearea*2.0f);
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
}

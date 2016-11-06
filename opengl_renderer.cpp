//
//  opengl_renderer.cpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "opengl_renderer.hpp"

// This will be the only place gl.h is mentioned.
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

struct OpenGLRendererImpl
{
    SDL_GLContext context = NULL;
    SDL_Window* window = NULL;
	GLuint height_map = 0;
	GLuint frag = 0;
	GLuint vert = 0;
	GLuint pipeline = 0;
	GLuint params = 0;
	GLuint vao = 0;
	GLuint vbo = 0;
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
	
    const char* vert_list[1] = {""};
    const char* frag_list[1] = {""};
	
    impl->vert = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, vert_list);
    impl->frag = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, frag_list);
    glUniformBlockBinding(impl->vert, 0, 0);
    glUniformBlockBinding(impl->vert, 1, 1);
    glUniformBlockBinding(impl->frag, 0, 0);
    glUniformBlockBinding(impl->frag, 1, 3);
	
	glGenProgramPipelines(1, &impl->pipeline);
    glUseProgramStages(impl->pipeline, GL_VERTEX_SHADER_BIT, impl->vert);
    glUseProgramStages(impl->pipeline, GL_FRAGMENT_SHADER_BIT, impl->frag);
 
	glGenVertexArrays(1, &impl->vao);
    glBindVertexArray(impl->vao);
	
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers);
    glViewport(0, 0, 800, 600); //

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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

void OpenGLRenderer::setHeightMap(void* pixels, int width, int height)
{
    if (this->impl->height_map == 0) {
        glGenTextures(1, &this->impl->height_map);
	}
	glBindTexture(GL_TEXTURE_BUFFER, this->impl->height_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	
    if (this->impl->vbo == 0) {
        glGenBuffers(1, &this->impl->vbo);
	}
	
	// Collapsed geosphere, just a triangle.
	float vertex_data[] = { 0.0, 0.0,-0.5,
	                       -0.5, 0.0, 0.5,
				            0.5, 0.0, 0.5};
	
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->impl->vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
}

void OpenGLRenderer::setParameters(const WorldParameters& params)
{
    if (this->impl->params == 0) {
        glGenBuffers(1, &this->impl->params);
	}
    glBindBuffer(GL_UNIFORM_BUFFER, impl->params);
    glBufferData(GL_UNIFORM_BUFFER, 65536, nullptr, GL_STATIC_DRAW);
}

void OpenGLRenderer::render(const PlayerPosition& pos)
{
    // The state should not have changed.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
	SDL_GL_SwapWindow(impl->window);
}

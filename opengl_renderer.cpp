//
//  opengl_renderer.cpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "opengl_renderer.hpp"

#include <sstream>

// This will be the only place gl.h is mentioned.
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "icosphere.hpp"

struct OpenGLRendererImpl
{
    SDL_GLContext context = NULL;
    SDL_Window* window = NULL;
    GLuint height_map = 0;
    GLuint frag = 0;
    GLuint vert = 0;
    GLuint pipeline = 0;
    GLuint params = 0;
    GLuint pos = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    
    GLuint num_indicies = 0;
};

inline char* load_file(const char* name)
{
    char* code = NULL;
    
    char* base_path = SDL_GetBasePath();
    std::stringstream temp_file;
    temp_file << base_path;
    temp_file << name;
    std::string file_str = temp_file.str();
    
    SDL_RWops* data = SDL_RWFromFile(file_str.c_str(), "rb");
    if (data != NULL) {
        Sint64 size = SDL_RWsize(data);
        code = (char*) SDL_malloc(size);

        if (code != NULL) {
            Sint64 total = 0;
            Sint64 read = 0;
            while (total < size &&
                   (read = SDL_RWread(data,
                                      code + read, 1, (size - total))) > 0) {
                total += read;
            }
        }
        code[size - 1] = 0;
        
        SDL_RWclose(data);
    }
    
    return code;
}


// TODO All window creation per renderer for now, should be a separate abstraction.
OpenGLRenderer::OpenGLRenderer()
{
    this->impl = new OpenGLRendererImpl();
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
 
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);

    impl->window = SDL_CreateWindow("Walking on the Moon",
                       SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED,
                       800, 600,
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
    
    glGenVertexArrays(1, &impl->vao);
    glBindVertexArray(impl->vao);
    
    char* vert_code = load_file("moonshader.vert");
    char* frag_code = load_file("moonshader.frag");
    
    const char* vert_list[1] = {vert_code};
    const char* frag_list[1] = {frag_code};
    
    impl->vert = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, vert_list);
    impl->frag = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, frag_list);
    
    SDL_free(vert_code);
    SDL_free(frag_code);
    
    glUniformBlockBinding(impl->vert, 0, 0);
    glUniformBlockBinding(impl->vert, 1, 1);
    //glUniformBlockBinding(impl->frag, 0, 0);
    glBindFragDataLocation(impl->frag, 0, "rt0");
    
    glGenProgramPipelines(1, &impl->pipeline);
    glUseProgramStages(impl->pipeline, GL_VERTEX_SHADER_BIT, impl->vert);
    glUseProgramStages(impl->pipeline, GL_FRAGMENT_SHADER_BIT, impl->frag);
    
    glGenBuffers(1, &this->impl->params);
    glBindBuffer(GL_UNIFORM_BUFFER, impl->params);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(WorldParameters), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, impl->params);
    
    glGenBuffers(1, &this->impl->pos);
    glBindBuffer(GL_UNIFORM_BUFFER, impl->pos);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PlayerPosition), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, impl->pos);
    
    glBindProgramPipeline(impl->pipeline);
    
    glViewport(0, 0, 800, 600); //

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
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

void OpenGLRenderer::setHeightMap(void* pixels, int width, int height, int bpp)
{
    if (this->impl->height_map == 0) {
        glGenTextures(1, &this->impl->height_map);
    }
    glBindTexture(GL_TEXTURE_2D, this->impl->height_map);
	
    GLenum format = GL_NONE;
    switch (bpp) {
        case 3: format = GL_RGB;
        case 4: format = GL_RGBA;
    }
	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
}

void OpenGLRenderer::setParameters(const WorldParameters& params)
{
    glBindBuffer(GL_UNIFORM_BUFFER, impl->params);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(params), &params, GL_STATIC_DRAW);
    
    VertexArray array = create_sphere_segment(params.radius, params.pixelsperdegree);

    impl->num_indicies = array.num_indicies;
 
    if (this->impl->vbo == 0) {
        glGenBuffers(1, &this->impl->vbo);
    }
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, this->impl->vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    
    glBufferData(GL_ARRAY_BUFFER, array.length, array.data, GL_STATIC_DRAW);
    
    if (this->impl->ibo == 0) {
        glGenBuffers(1, &this->impl->ibo);
    }
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->impl->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, array.index_length, array.index_data, GL_STATIC_DRAW);
    
    delete[] array.data;
    delete[] array.index_data;
}

void OpenGLRenderer::render(const PlayerPosition& pos)
{
    glBindBuffer(GL_UNIFORM_BUFFER, impl->pos);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(pos), &pos, GL_DYNAMIC_DRAW);
    
    // The state should not have changed.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glDrawElements(GL_TRIANGLES, impl->num_indicies, GL_UNSIGNED_INT, 0);
    SDL_GL_SwapWindow(impl->window);
}

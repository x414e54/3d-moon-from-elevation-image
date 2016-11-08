//
//  metal_renderer.cpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "metal_renderer.hpp"

#import <Metal/Metal.h>
#import <AppKit/AppKit.h>
#import <MetalKit/MetalKit.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

struct MetalRendererImpl
{
    SDL_Window* window = NULL;
    MTKView* metal_view;
    
    id <MTLDevice> device;
    id <MTLCommandQueue> cq;
    id <MTLCommandBuffer> cb;
    id <MTLLibrary> library;
    
    id <MTLTexture> height_map;
    id <MTLFunction> frag;
    id <MTLFunction> vert;
    id <MTLRenderPipelineState> pipeline;
    id <MTLBuffer> params;
    id <MTLBuffer> pos;
    id <MTLBuffer> vertex;
};

const char* MetalRenderer::type_name = "Metal";

const char* MetalRenderer::get_name()
{
    return MetalRenderer::type_name;
}

MetalRenderer::MetalRenderer()
{
    this->impl = new MetalRendererImpl();
    
    impl->window = SDL_CreateWindow("Walking on the Moon",
                       SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED,
                       800, 600,
                       0);
    if (!impl->window) {
        // TODO Add some way to fail.
        return;
    }
    
    impl->device = MTLCreateSystemDefaultDevice();
    
    struct SDL_SysWMinfo info;
    SDL_GetWindowWMInfo(impl->window, &info);
    
    NSView *view = info.info.cocoa.window.contentView;

    impl->metal_view = [[MTKView alloc] initWithFrame:view.frame device:impl->device];
    [view addSubview:impl->metal_view];
    
    impl->library = [impl->device newDefaultLibrary];
    impl->vert = [impl->library newFunctionWithName:@"moon_vertex"];
    impl->frag = [impl->library newFunctionWithName:@"moon_fragment"];
    
    MTLRenderPipelineDescriptor *pipeline_desc = [MTLRenderPipelineDescriptor new];
    pipeline_desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipeline_desc.sampleCount = impl->metal_view.sampleCount;
    pipeline_desc.vertexFunction = impl->vert;
    pipeline_desc.fragmentFunction = impl->frag;
    
    /*impl->pipeline = [impl->device newRenderPipelineStateWithDescriptor:pipeline_desc];
    
    impl->params = [impl->device newBufferWithLength: sizeof(WorldParameters)];
    impl->pos = [impl->device newBufferWithLength: sizeof(PlayerPosition)];*/
    
    [impl->metal_view setClearColor: MTLClearColorMake(1.0, 0.0, 0.0, 1.0)];
}

MetalRenderer::~MetalRenderer()
{
    [impl->metal_view removeFromSuperview];
    delete impl;
    SDL_DestroyWindow(impl->window);
}

void MetalRenderer::setHeightMap(void* pixels, int width, int height, int bpp)
{
    MTLTextureDescriptor *tex_desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatR8Unorm
                                      width:width
                                      height:height
                                      mipmapped:NO];

    impl->height_map = [impl->device newTextureWithDescriptor:tex_desc];
    
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    [impl->height_map replaceRegion:region
     mipmapLevel:0
     withBytes:pixels
     bytesPerRow:width * bpp];
    
    // Collapsed geosphere, just a triangle.
    float vertex_data[] = { 0.0, 0.0,-0.5,
                           -0.5, 0.0, 0.5,
                            0.5, 0.0, 0.5};
    impl->vertex = [impl->device newBufferWithBytes:vertex_data length:sizeof(vertex_data) options:MTLResourceOptionCPUCacheModeDefault];
    
    /*[impl->encoder setVertexBuffer:impl->vertex
                       offset:0
                       atIndex:0];*/
}

void MetalRenderer::setParameters(const WorldParameters& params)
{
    impl->params = [impl->device newBufferWithBytes:&params length:sizeof(params) options:MTLResourceOptionCPUCacheModeDefault];
}

void MetalRenderer::render(const PlayerPosition& pos)
{
    impl->pos = [impl->device newBufferWithBytes:&pos length:sizeof(pos) options:MTLResourceOptionCPUCacheModeDefault];
    [impl->cb presentDrawable: impl->metal_view.currentDrawable];
    [impl->cb commit];
    [impl->metal_view draw];
}

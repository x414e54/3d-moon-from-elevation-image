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

@interface DrawDelegate : NSObject <MTKViewDelegate> {
    MetalRendererImpl* _impl;
}

@property (nonatomic) MetalRendererImpl* impl;
@end

struct MetalRendererImpl
{
    SDL_Window* window = NULL;
    MTKView* metal_view;
    
    id <MTLDevice> device;
    id <MTLCommandQueue> cq;
    id <MTLLibrary> library;
    
    id <MTLSamplerState> terrain_sampler;
    id <MTLTexture> terrain;
    id <MTLTexture> height_map;
    id <MTLFunction> frag;
    id <MTLFunction> vert;
    id <MTLRenderPipelineState> pipeline;
    id <MTLBuffer> vertex;
    
    const WorldParameters* params;
    const PlayerPosition* pos;
    
    DrawDelegate* delegate;
};

const char* MetalRenderer::type_name = "Metal";

const char* MetalRenderer::get_name()
{
    return MetalRenderer::type_name;
}

@implementation DrawDelegate

@synthesize impl = _impl;

- (void)mtkView:(MTKView*)view
drawableSizeWillChange:(CGSize)size
{

}

- (void)encode:(id <MTLCommandBuffer>)cb
{
    id <MTLRenderCommandEncoder> encoder = [cb renderCommandEncoderWithDescriptor:_impl->metal_view.currentRenderPassDescriptor];
    [encoder setRenderPipelineState:_impl->pipeline];
    [encoder setVertexTexture:_impl->height_map
                 atIndex:0];
    [encoder setFragmentTexture:_impl->height_map
                 atIndex:0];
    [encoder setFragmentSamplerState:_impl->terrain_sampler
                 atIndex:0];
    [encoder setVertexBuffer:_impl->vertex
                 offset:0
                 atIndex:0];
    [encoder setVertexBytes:_impl->params length:sizeof(WorldParameters)
                 atIndex:1];
    [encoder setVertexBytes:_impl->pos length:sizeof(PlayerPosition)
                 atIndex:2];
    [encoder setFragmentBytes:_impl->params length:sizeof(WorldParameters)
                 atIndex:0];
    [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                 vertexStart:0
                 vertexCount:3
                 instanceCount:1];
    [encoder endEncoding];
}

- (void)drawInMTKView:(MTKView*)view
{
    id <MTLCommandBuffer> cb = [_impl->cq commandBuffer];
    [self encode: cb];
    [cb presentDrawable: _impl->metal_view.currentDrawable];
    [cb commit];
}

@end

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
    
    NSView* view = info.info.cocoa.window.contentView;

    impl->metal_view = [[MTKView alloc] initWithFrame:view.frame device:impl->device];
    [impl->metal_view setPaused:true];
    [impl->metal_view setNeedsDisplay:false];
    impl->delegate = [[DrawDelegate alloc] init];
    [impl->delegate setImpl: impl];
    [impl->metal_view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [view addSubview:impl->metal_view];
    
    impl->library = [impl->device newDefaultLibrary];
    impl->vert = [impl->library newFunctionWithName:@"moon_vertex"];
    impl->frag = [impl->library newFunctionWithName:@"moon_fragment"];
    
    MTLRenderPipelineDescriptor* pipeline_desc = [MTLRenderPipelineDescriptor new];
    pipeline_desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipeline_desc.sampleCount = impl->metal_view.sampleCount;
    pipeline_desc.vertexFunction = impl->vert;
    pipeline_desc.fragmentFunction = impl->frag;
    
    NSError* err = nil;
    impl->pipeline = [impl->device newRenderPipelineStateWithDescriptor:pipeline_desc error:&err];
    
    MTLSamplerDescriptor* sampler_desc = [MTLSamplerDescriptor new];
    sampler_desc.sAddressMode = MTLSamplerAddressModeClampToZero;
    sampler_desc.tAddressMode = MTLSamplerAddressModeClampToZero;
    sampler_desc.minFilter = MTLSamplerMinMagFilterNearest;
    sampler_desc.magFilter = MTLSamplerMinMagFilterLinear;

    impl->terrain_sampler = [impl->device newSamplerStateWithDescriptor:sampler_desc];
    
    [impl->metal_view setClearColor: MTLClearColorMake(1.0, 0.0, 0.0, 1.0)];
    
    impl->cq = [impl->device newCommandQueue];
    
    [impl->metal_view setDelegate:impl->delegate];
}

MetalRenderer::~MetalRenderer()
{
    [impl->metal_view removeFromSuperview];
    delete impl;
    SDL_DestroyWindow(impl->window);
}

void MetalRenderer::setHeightMap(void* pixels, int width, int height, int bpp)
{
    assert(bpp == 4); // Update this later.
    MTLTextureDescriptor* tex_desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                      width:width
                                      height:height
                                      mipmapped:NO];

    impl->height_map = [impl->device newTextureWithDescriptor:tex_desc];
    impl->terrain = impl->height_map;
    
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    [impl->height_map replaceRegion:region
     mipmapLevel:0
     withBytes:pixels
     bytesPerRow:width * bpp];
    
    // Collapsed geosphere, just a triangle.
    float vertex_data[] = /*{ 0.0, 0.0,-0.5,
                           -0.5, 0.0, 0.5,
                            0.5, 0.0, 0.5};*/
                            // Test fullscreen triangle
                        { 0.0,  1.0, 0.0,
                         -1.0, -1.0, 0.0,
                          1.0, -1.0, 0.0};
    impl->vertex = [impl->device newBufferWithBytes:vertex_data length:sizeof(vertex_data) options:MTLResourceOptionCPUCacheModeDefault];
}

void MetalRenderer::setParameters(const WorldParameters& params)
{
    impl->params = &params; // Currently assuming params is valid for lifetime of renderer
}

void MetalRenderer::render(const PlayerPosition& pos)
{
    impl->pos = &pos;
    
    [impl->metal_view draw];
}

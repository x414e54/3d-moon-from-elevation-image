//
//  metal_renderer.cpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "metal_renderer.hpp"

const char* MetalRenderer::type_name = "Metal";

const char* MetalRenderer::get_name()
{
    return MetalRenderer::type_name;
}

void MetalRenderer::setHeightMap(void* pixels, int width, int height, int bpp)
{
}

void MetalRenderer::setParameters(const WorldParameters& params)
{
}

void MetalRenderer::render(const PlayerPosition& pos)
{
}

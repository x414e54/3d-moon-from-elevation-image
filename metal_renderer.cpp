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

int MetalRenderer::createList(std::vector<Vertex*> &list, float offset, int radius)
{
    // Will be unused for metal, use vertex buffer instead?
	return -1;
}

void MetalRenderer::updateList(std::vector<Vertex*> &list, int index, float offset, int radius)
{
    // Will be unused for metal, use vertex buffer instead?
}

void MetalRenderer::render(int index, const PlayerPosition& pos, int anglearea, int pixelsperdegree)
{
}

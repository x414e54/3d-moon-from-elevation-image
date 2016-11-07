//
//  metal_renderer.hpp
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#ifndef metal_renderer_hpp
#define metal_renderer_hpp

#include "renderer.h"

class MetalRenderer : public Renderer
{
public:
    static const char* type_name;
    virtual const char* get_name();
	
	virtual void setHeightMap(void* pixels, int width, int height, int bpp);
	virtual void setParameters(const WorldParameters& params);
	virtual void render(const PlayerPosition& pos);
};

#endif /* metal_renderer_hpp */

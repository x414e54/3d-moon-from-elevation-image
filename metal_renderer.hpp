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
	
	virtual int createList(std::vector<Vertex*> &list, float offset, int radius);
	virtual void updateList(std::vector<Vertex*> &list, int index, float offset, int radius);
	virtual void render(int index, const PlayerPosition& pos, int anglearea, int pixelsperdegree);
};

#endif /* metal_renderer_hpp */

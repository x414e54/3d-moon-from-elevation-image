//
//  renderer.h
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#ifndef renderer_h
#define renderer_h

#include "geometry.h"

// TODO Move to renderer parameter
extern float MAXHEIGHTRANGE;
extern HeightMap heightMap;

struct PlayerPosition
{
	float inclination;	
	float azimuth;
	float radius;
	float viewheight;
	float orientation;
	float speed;
	void update(float timeDelta)
	{
		azimuth+=speed*(timeDelta);
	}
};

class Renderer
{
public:
    Renderer() {}
	virtual ~Renderer() {}
	
    virtual const char* get_name() = 0;
	
	virtual int createList(std::vector<Vertex*> &list, float offset, int radius) = 0;
	virtual void updateList(std::vector<Vertex*> &list, int index, float offset, int radius) = 0;
	virtual void render(int index, const PlayerPosition& pos, int anglearea, int pixelsperdegree) = 0;
};

#endif /* renderer_h */

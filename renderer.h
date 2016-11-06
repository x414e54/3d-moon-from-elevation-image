//
//  renderer.h
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#ifndef renderer_h
#define renderer_h

#define PI 3.14159265358979

struct WorldParameters
{
	float MAXHEIGHTRANGE;
	int width;
	int height;
	int anglearea;
	int pixelsperdegree;
};

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
	
	virtual void setHeightMap(void* pixels, int width, int height) = 0;
	virtual void setParameters(const WorldParameters& params) = 0;
	virtual void render(const PlayerPosition& pos) = 0;
};

#endif /* renderer_h */

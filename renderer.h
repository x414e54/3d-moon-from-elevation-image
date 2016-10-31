//
//  renderer.h
//  moon
//
//  Created by x414e54 on 2016/10/31.
//  Copyright © 2016 x414e54. All rights reserved.
//

#ifndef renderer_h
#define renderer_h

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
    virtual const char* get_name() = 0;
	virtual void render(int index, const PlayerPosition& pos, int anglearea, int pixelsperdegree);
};

#endif /* renderer_h */

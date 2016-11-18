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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct WorldParameters
{
	float MAXHEIGHTRANGE;
	int width;
	int height;
	int anglearea;
	int pixelsperdegree;
	float radius;
};

struct PlayerPosition
{
    glm::mat4x4 view_projection;
	glm::vec2 heightmap_offset_uv;
	float viewheight;
	float rotation_1;
	float rotation_2;
    
	float speed;
    float radius;
    int control_1;
    int control_2;
    glm::mat4x4 projection;
    glm::mat4x4 view;
	void update(float timeDelta)
	{
		heightmap_offset_uv.y+=speed*(timeDelta);
        
        rotation_1+=speed*(timeDelta)*control_1;
        rotation_2+=speed*(timeDelta)*control_2;
        
        glm::mat4 temp = glm::translate(glm::mat4(),
                             glm::vec3(0.0, radius + viewheight, 0.0));
        temp = glm::rotate(temp, (float)PI/6.0f, glm::vec3(1, 0, 0));
        temp = glm::rotate(temp, rotation_1, glm::vec3(0, 1, 0));
        temp = glm::rotate(temp, rotation_2, glm::vec3(1, 0, 0));

        view = temp;//glm::inverse(temp);
        view_projection = projection * view;
	}
};

class Renderer
{
public:
    Renderer() {}
	virtual ~Renderer() {}
	
    virtual const char* get_name() = 0;
	
	virtual void setHeightMap(void* pixels, int width, int height, int bpp) = 0;
	virtual void setParameters(const WorldParameters& params) = 0;
	virtual void render(const PlayerPosition& pos) = 0;
};

#endif /* renderer_h */

#version 150 
#extension GL_ARB_separate_shader_objects: require
#extension GL_ARB_explicit_attrib_location: require

layout(location = 0) in vec3 position;

constant float PI = 3.14159265358979;

struct WorldParameters
{
    float MAXHEIGHTRANGE;
    int width;
    int height;
    int anglearea;
    int pixelsperdegree;
	float radius;
};

struct PlayerParameters
{
    mat4 view_projection;
	vec2 heightmap_offset_uv;
	float viewheight;
	float rotation_1;
	float rotation_2;
};

uniform sampler2D heightmap;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

out vec2 vertex_texcoord;

float getHeightFromColor(float r, float max)
{
    return (max*r*2.0f/255.0f) - max;
}

vec2 findUV(vec3 pos, vec2 offset)
{
    // Calculate u v
    float u = 0.5 + (atan2(pos.z, pos.x) / (2*PI));
    float v = 0.5 - (2.0 * (asin(pos.y) / (2*PI)));

    return fract(vec2(u, v) + offset);
}

void main()
{
    gl_Position = vec4(position.xzy, 1.0);
    vertex_texcoord = vec2(0.0, 0.0);

    
    VertexOut out;
    vec2 uv = findUV(normalize(position.xzy), pos.heightmap_offset_uv);
    int ix = round(uv.x*(params.width - 1));
	int iy = round(uv.y*(params.height - 1));
    
    //Finds the height from a uv heightmap
    float height = texture(heightmap, uint2(ix, iy)).r;
	height = getHeightFromColor(height, params.MAXHEIGHTRANGE);
    
    gl_Position = pos.view_projection * vec4(position.xzy, 1.0);
    vertex_texcoord = uv;
}


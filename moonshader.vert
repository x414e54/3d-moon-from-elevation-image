#version 150 
#extension GL_ARB_separate_shader_objects: require
#extension GL_ARB_explicit_attrib_location: require

layout(location = 0) in vec3 position;

float PI = 3.14159265358979;

uniform WorldParameters
{
    float MAXHEIGHTRANGE;
    int width;
    int height;
    int anglearea;
    int pixelsperdegree;
    float radius;
};

uniform PlayerParameters
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
    float u = 0.5 + (atan(pos.z, pos.x) / (2*PI));
    float v = 0.5 - (2.0 * (asin(pos.y) / (2*PI)));

    return fract(vec2(u, v) + offset);
}

void main()
{
    vec2 uv = findUV(normalize(position.xzy), heightmap_offset_uv);
    int ix = int(round(uv.x*(float(width) - 1.0)));
    int iy = int(round(uv.y*(float(height) - 1.0)));
    
    //Finds the height from a uv heightmap
    float height = texture(heightmap, ivec2(ix, iy)).r;
    height = getHeightFromColor(height, MAXHEIGHTRANGE);
    
    gl_Position = view_projection * vec4(position.xzy, 1.0);
    vertex_texcoord = uv;
}


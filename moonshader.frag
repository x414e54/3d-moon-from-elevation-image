#version 150 
#extension GL_ARB_separate_shader_objects: require
#extension GL_ARB_explicit_attrib_location: require

uniform WorldParameters
{
    float MAXHEIGHTRANGE;
    int width;
    int height;
    int anglearea;
    int pixelsperdegree;
};

uniform sampler2D heightmap;

in vec2 vertex_texcoord;

out vec4 rt0;

void main()
{
    vec4 texel = texture(heightmap, vertex_texcoord);
    rt0 = vec4(texel.rgb, 1.0);
}

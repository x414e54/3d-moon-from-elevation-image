#version 150 
#extension GL_ARB_separate_shader_objects: require
#extension GL_ARB_explicit_attrib_location: require
#extension GL_ARB_explicit_uniform_location: require
#extension GL_ARB_shading_language_420pack: require

layout(std140, binding = 0) uniform WorldParameters
{
    mat4 view;
    mat4 projection;
    mat4 view_projection;
	float MAXHEIGHTRANGE;
	int width;
	int height;
	int anglearea;
	int pixelsperdegree;
};

layout(binding = 0) uniform sampler2D heightmap;

layout(location = 1) in vec3 vertex_position;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec2 vertex_texcoord; 

layout(location = 0) out vec4 rt0;

void main()
{
	vec4 texel = texture(heightmap, vertex_texcoord);
    rt0 = vec4(texel.rgb, 1.0);
}

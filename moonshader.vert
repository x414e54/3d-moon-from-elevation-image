#version 150 
#extension GL_ARB_separate_shader_objects: require
#extension GL_ARB_explicit_attrib_location: require

layout(location = 0) in vec3 position;

uniform WorldParameters
{
	float MAXHEIGHTRANGE;
	int width;
	int height;
	int anglearea;
	int pixelsperdegree;
};

uniform PlayerParameters
{
	float inclination;	
	float azimuth;
	float radius;
	float viewheight;
	float orientation;
};

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

out vec2 vertex_texcoord;

void main()
{
    gl_Position = vec4(position.xzy, 1.0);
	vertex_texcoord = vec2(0.0, 0.0);
}


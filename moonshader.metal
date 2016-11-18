//
//  moonshader.metal
//  moon
//
//  Created by x414e54 on 2016/11/05.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

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
    float4x4 view_projection;
	float orientation;
	float rotation_1;
	float rotation_2;
	float heightmap_offset_u;
	float heightmap_offset_v;
	float viewheight;
};

struct VertexIn
{
    packed_float3 position;
    packed_float3 normal;
};

struct VertexOut
{
    float4 position[[position]];
    float3 normal;
    float2 texcoord;
};

inline float getHeightFromColor(float r, float max)
{
	return (max*r*2.0f/255.0f) - max;
}

inline float2 findUV(float3 pos)
{
	// Calculate u v
	float u = 0.5 + (atan2(pos.z, pos.x) / (2*PI));
	float v = 0.5 - (2.0 * (asin(pos.y) / (2*PI)));

    return float2(u, v);
}

vertex VertexOut moon_vertex(device packed_float3* position [[buffer(0)]],
                             constant WorldParameters& params [[buffer(1)]],
                             constant PlayerParameters& pos [[buffer(2)]],
                             texture2d<float, access::read> height_map [[texture(0)]],
                             uint vertexID [[vertex_id]])
{
    VertexOut out;
    float2 uv = findUV(normalize(position[vertexID]));
    int ix = round(uv.x*(params.width - 1));
	int iy = round(uv.y*(params.height - 1));
    
    //Finds the height from a uv heightmap
    float height = height_map.read(uint2(ix, iy)).r;
	height = getHeightFromColor(height, params.MAXHEIGHTRANGE);
    
    out.position = pos.view_projection * float4(position[vertexID], 1.0);
    out.texcoord = uv;
    return out;
}

fragment half4 moon_fragment(VertexOut fragment_in [[stage_in]],
                             texture2d<float, access::sample> terrain [[texture(0)]],
                             sampler terrain_sampler [[sampler(0)]])
{
    float4 color = terrain.sample(terrain_sampler, fragment_in.texcoord);
    color.a = 1.0;
    return half4(color.rrra);
}

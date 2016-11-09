//
//  moonshader.metal
//  moon
//
//  Created by x414e54 on 2016/11/05.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

struct WorldParameters
{
    float MAXHEIGHTRANGE;
    int width;
    int height;
    int anglearea;
    int pixelsperdegree;
};

struct PlayerParameters
{
    float inclination;	
    float azimuth;
    float radius;
    float viewheight;
    float orientation;
};

struct VertexOut
{
    float4 position[[position]];
    float3 normal;
    float2 texcoord;
};

vertex VertexOut moon_vertex(device packed_float3 &position [[buffer(0)]],
                          constant WorldParameters &params [[buffer(1)]],
                          constant PlayerParameters &pos [[buffer(2)]],
                          texture2d<float, access::read> height_map [[texture(0)]],
                          uint vertexID [[vertex_id]])
{
    VertexOut out;
    float height = height_map.read(uint2(0)).r;
    return out;
}

fragment half4 moon_fragment(VertexOut fragment_in [[stage_in]],
                             texture2d<float, access::sample> terrain [[texture(0)]],
                             sampler terrain_sampler [[sampler(0)]])
{
    float4 color = terrain.sample(terrain_sampler, fragment_in.texcoord);
    return half4(color);
}

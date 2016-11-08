//
//  moonshader.metal
//  moon
//
//  Created by x414e54 on 2016/11/05.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

vertex float4 moon_vertex()
{
   return float4(1.0, 0.0, 0.0, 1.0);
}

fragment half4 moon_fragment()
{
   return half4(1.0, 0.0, 0.0, 1.0);
}

//
//  icosphere.hpp
//  moon
//
//  Created by x414e54 on 2016/11/13.
//  Copyright © 2016 x414e54. All rights reserved.
//

#ifndef icosphere_hpp
#define icosphere_hpp

struct VertexArray
{
    float* data;
    int length; // Should be 64bit later
    int* index_data;
    int index_length; // Should be 64bit later
};

extern VertexArray create_sphere_segment(float radius, float ppd);

#endif /* icosphere_hpp */

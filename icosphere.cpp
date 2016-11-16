//
//  icosphere.cpp
//  moon
//
//  Created by x414e54 on 2016/11/13.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "icosphere.hpp"

#include <cmath>

#include "renderer.h"

struct Vector3
{
    float x;
    float y;
    float z;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Matrix3
{
    Vector3 a;
    Vector3 b;
    Vector3 c;
    Matrix3(Vector3 a, Vector3 b, Vector3 c) : a(a), b(b), c(c) {}
};

inline Vector3 rotate_vertex(const Vector3& vertex, const Matrix3& rotation)
{
    return Vector3((vertex.x * rotation.a.x) + (vertex.y * rotation.b.x) + (vertex.z * rotation.c.x),
                   (vertex.x * rotation.a.y) + (vertex.y * rotation.b.y) + (vertex.z * rotation.c.y),
                   (vertex.x * rotation.a.z) + (vertex.y * rotation.b.z) + (vertex.z * rotation.c.z));
}

inline Matrix3 make_rotation_matrix(const Vector3& axis, float rotation)
{
    float sin_angle = std::sinf(rotation);
    float cos_angle = std::cosf(rotation);
    return Matrix3(Vector3(cos_angle + (axis.x*axis.x*(1-cos_angle)),
                           (axis.y * axis.x * (1-cos_angle)) + axis.z * sin_angle,
                           (axis.z * axis.x * (1-cos_angle)) + axis.y * sin_angle),
                   
                   Vector3((axis.x * axis.y * (1-cos_angle)) + axis.z * sin_angle,
                           cos_angle + (axis.y*axis.y*(1-cos_angle)),
                           (axis.z * axis.y * (1-cos_angle)) + axis.x * sin_angle),
                   
                   Vector3((axis.x * axis.z * (1-cos_angle)) + axis.y * sin_angle,
                           (axis.y * axis.z * (1-cos_angle)) + axis.x * sin_angle,
                           cos_angle + (axis.z*axis.z*(1-cos_angle))));
}

inline float calc_radians_per_pixel(float ppd)
{
    return PI/(180.0 * ppd);
}

VertexArray create_sphere_segment(float radius, float ppd)
{
    float arc_angle = 360/4;
    // Debug
    ppd = (1.0 + std::sqrtf(2.0)) / arc_angle;
    radius = 1.0;
    // End Debug
    int num_pixels = ppd * arc_angle;
    int num_verticies = ((num_pixels * num_pixels) + num_pixels) / 2.0;
    int num_faces = ((num_pixels * num_pixels) - (2.0 * num_pixels)) + 1.0;
    
    VertexArray array;
    array.num_indicies = 3 * num_faces;
    array.length = num_verticies * sizeof(float) * 3;
    array.data = new float[array.length];
    array.index_length = array.num_indicies * sizeof(int);
    array.index_data = new int[array.index_length];
    
    float rot_cos = std::cosf(PI / 6.0);
    float rot_sin = std::sinf(PI / 6.0);
    float rpp = calc_radians_per_pixel(ppd);
    
    Matrix3 col = make_rotation_matrix(Vector3(rot_cos, 0.0, -rot_sin), rpp);
    Matrix3 row = make_rotation_matrix(Vector3(-rot_cos, 0.0, -rot_sin), rpp);
    
    Vector3 start(0.0, radius, 0.0);
    Vector3 vertex(0.0, radius, 0.0);
    
    int vertex_index = 0;
    int col_length = 0;
    for (int i = 0; i < num_pixels; ++i) {
        col_length = num_pixels - i;
        for (int j = 0; j < col_length; ++j) {
            array.data[vertex_index] = vertex.x;
            array.data[vertex_index + 1] = vertex.y;
            array.data[vertex_index + 2] = vertex.z;
            vertex_index += 3;
            vertex = rotate_vertex(vertex, row);
        }
        start = rotate_vertex(start, col);
        vertex = start;
    }
    
    for (int i = 0; i < array.num_indicies; ++i) {
        array.index_data[i] = i;
    }
    
    return array;
}

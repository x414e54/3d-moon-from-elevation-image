//
//  icosphere.cpp
//  moon
//
//  Created by x414e54 on 2016/11/13.
//  Copyright © 2016 x414e54. All rights reserved.
//

#include "icosphere.hpp"

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
    return vertex;
}

inline Matrix3 make_rotation_matrix(const Vector3& axis, float rotation)
{
    return Matrix3(axis, axis, axis);
}

VertexArray create_sphere_segment(float radius, float ppd)
{
    float arc_angle = 360/4;
    float num_pixels = ppd * arc_angle;    float num_verticies = ((num_pixels * num_pixels) + num_pixels) / 2.0;
    float num_faces = ((num_pixels * num_pixels) - (2.0 * num_pixels));
    float num_indicies = 3 * num_faces;
    
    VertexArray array;
    array.length = num_verticies * sizeof(float);
    array.data = new float[array.length];
    array.index_length = num_indicies * sizeof(int);
    array.index_data = new int[array.index_length];
    
    Matrix3 col = make_rotation_matrix(Vector3(0.0, 0.0, 0.0), 1.0 / ppd);
    Matrix3 row = make_rotation_matrix(Vector3(0.0, 0.0, 0.0), 1.0 / ppd);
    
    Vector3 start(0.0, 0.0, 0.0);
    Vector3 vertex(0.0, 0.0, 0.0);
    
    int vertex_index = 0;
    int col_length = 0;
    for (int i = 0; i < num_pixels; ++i) {
        col_length = num_pixels - i;
        for (int j = 0; j < col_length; ++j) {
            array.data[vertex_index] = vertex.x;
            array.data[vertex_index + 2] = vertex.y;
            array.data[vertex_index + 3] = vertex.z;
            vertex_index += 3;
            vertex = rotate_vertex(vertex, row);
        }
        start = rotate_vertex(start, col);
        vertex = start;
    }
    
    for (int i = 0; i < num_faces; ++i) {
        
    }
    
    return array;
}

#include <math.h>
#include <iostream>
#include "geometry.h"
#include <cstdlib>

#if __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/*

Adaptation of freeglut solidsphere for this example

*/

/*
 * freeglut_geometry.c
 *
 * Freeglut geometry rendering methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Compute lookup table of cos and sin values forming a cirle
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */

static void fghCircleTable(double **sint,double **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*M_PI/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        //fgError("Failed to allocate memory in fghCircleTable");
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}


/*
 * Draws a solid sphere
 */
void createSphereDome(std::vector<Vertex*>* list, double radius, int slices, int stacks)
{
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

    for( i=1; i<stacks-1; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        //glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
				list->push_back(new Vertex(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius, cost1[j]*r1, sint1[j]*r1, z1));
				list->push_back(new Vertex(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius, cost1[j]*r0, sint1[j]*r0, z0));
               // glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                //glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                //glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                //glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            }

        //glEnd();
    }

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}


/*
 * Draws a solid sphere
 */
void createSphereDome(std::vector<Vertex*>* list, double radius, int slices, int stacks, int anglearea)
{
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

	int stackrange = stacks/360*anglearea;
	int centerstack = stacks/2;
	int centerslice = slices/2;
	int slicerange = slices/360*anglearea/2.0f;
    for( i=centerstack-stackrange; i<centerstack+stackrange; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        //glBegin(GL_QUAD_STRIP);

            for(j=centerslice-slicerange; j<=centerslice+slicerange; j++)
            {
				list->push_back(new Vertex(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius, cost1[j]*r1, sint1[j]*r1, z1));
				list->push_back(new Vertex(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius, cost1[j]*r0, sint1[j]*r0, z0));
               // glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                //glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                //glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                //glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            }

        //glEnd();
    }

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

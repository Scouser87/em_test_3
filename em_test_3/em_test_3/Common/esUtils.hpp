//
//  esUtils.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef esUtils_hpp
#define esUtils_hpp

#include <stdio.h>
#include <GLES/gl.h>

typedef struct
{
    union	{
        GLfloat   m[4][4];
        GLfloat   f[16];
    };
} ESMatrix;


void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);

void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);

void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);

void esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ);

void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);

void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB);

void esMatrixLoadIdentity(ESMatrix *result);

void esMatrixInverse  ( ESMatrix	*mOut, const ESMatrix	*mIn);

void  esMatrixTranspose( ESMatrix	*mOut, const ESMatrix	*mIn);

#endif /* esUtils_hpp */

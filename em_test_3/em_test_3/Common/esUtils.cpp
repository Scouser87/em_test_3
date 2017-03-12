//
//  esUtils.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "esUtils.hpp"

#include <math.h>
#include <memory.h>

#define PI 3.1415926535897932384626433832795f
#define _ABS(a)		((a) <= 0 ? -(a) : (a) )

void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;
    
    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;
    
    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat sinAngle, cosAngle;
    GLfloat mag = sqrtf(x * x + y * y + z * z);
    
    sinAngle = sinf ( angle * PI / 180.0f );
    cosAngle = cosf ( angle * PI / 180.0f );
    if ( mag > 0.0f )
    {
        GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
        GLfloat oneMinusCos;
        ESMatrix rotMat;
        
        x /= mag;
        y /= mag;
        z /= mag;
        
        xx = x * x;
        yy = y * y;
        zz = z * z;
        xy = x * y;
        yz = y * z;
        zx = z * x;
        xs = x * sinAngle;
        ys = y * sinAngle;
        zs = z * sinAngle;
        oneMinusCos = 1.0f - cosAngle;
        
        rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
        rotMat.m[0][1] = (oneMinusCos * xy) - zs;
        rotMat.m[0][2] = (oneMinusCos * zx) + ys;
        rotMat.m[0][3] = 0.0F;
        
        rotMat.m[1][0] = (oneMinusCos * xy) + zs;
        rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
        rotMat.m[1][2] = (oneMinusCos * yz) - xs;
        rotMat.m[1][3] = 0.0F;
        
        rotMat.m[2][0] = (oneMinusCos * zx) - ys;
        rotMat.m[2][1] = (oneMinusCos * yz) + xs;
        rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
        rotMat.m[2][3] = 0.0F;
        
        rotMat.m[3][0] = 0.0F;
        rotMat.m[3][1] = 0.0F;
        rotMat.m[3][2] = 0.0F;
        rotMat.m[3][3] = 1.0F;
        
        esMatrixMultiply( result, &rotMat, result );
    }
}

void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    frust;
    
    if ( (nearZ <= 0.0f) || (farZ <= 0.0f) ||
        (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f) )
        return;
    
    frust.m[0][0] = 2.0f * nearZ / deltaX;
    frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;
    
    frust.m[1][1] = 2.0f * nearZ / deltaY;
    frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;
    
    frust.m[2][0] = (right + left) / deltaX;
    frust.m[2][1] = (top + bottom) / deltaY;
    frust.m[2][2] = -(nearZ + farZ) / deltaZ;
    frust.m[2][3] = -1.0f;
    
    frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
    frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;
    
    esMatrixMultiply(result, &frust, result);
}


void esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
    GLfloat frustumW, frustumH;
    
    frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
    frustumW = frustumH * aspect;
    
    esFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float       deltaX = right - left;
    float       deltaY = top - bottom;
    float       deltaZ = farZ - nearZ;
    ESMatrix    ortho;
    
    if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
        return;
    
    esMatrixLoadIdentity(&ortho);
    ortho.m[0][0] = 2.0f / deltaX;
    ortho.m[3][0] = -(right + left) / deltaX;
    ortho.m[1][1] = 2.0f / deltaY;
    ortho.m[3][1] = -(top + bottom) / deltaY;
    ortho.m[2][2] = -2.0f / deltaZ;
    ortho.m[3][2] = -(nearZ + farZ) / deltaZ;
    
    esMatrixMultiply(result, &ortho, result);
}


void esMatrixMultiply(ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB)
{
    ESMatrix    tmp;
    int         i;
    
    for (i=0; i<4; i++)
    {
        tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
        (srcA->m[i][1] * srcB->m[1][0]) +
        (srcA->m[i][2] * srcB->m[2][0]) +
        (srcA->m[i][3] * srcB->m[3][0]) ;
        
        tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) +
        (srcA->m[i][1] * srcB->m[1][1]) +
        (srcA->m[i][2] * srcB->m[2][1]) +
        (srcA->m[i][3] * srcB->m[3][1]) ;
        
        tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) +
        (srcA->m[i][1] * srcB->m[1][2]) +
        (srcA->m[i][2] * srcB->m[2][2]) +
        (srcA->m[i][3] * srcB->m[3][2]) ;
        
        tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) +
        (srcA->m[i][1] * srcB->m[1][3]) +
        (srcA->m[i][2] * srcB->m[2][3]) +
        (srcA->m[i][3] * srcB->m[3][3]) ;
    }
    memcpy(result, &tmp, sizeof(ESMatrix));
}


void esMatrixLoadIdentity(ESMatrix *result)
{
    memset(result, 0x0, sizeof(ESMatrix));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}


void esMatrixTranspose( ESMatrix	*mOut, const ESMatrix	*mIn)
{
    static ESMatrix	mTmp;
    
    mTmp.f[ 0]=mIn->f[ 0];	mTmp.f[ 4]=mIn->f[ 1];	mTmp.f[ 8]=mIn->f[ 2];	mTmp.f[12]=mIn->f[ 3];
    mTmp.f[ 1]=mIn->f[ 4];	mTmp.f[ 5]=mIn->f[ 5];	mTmp.f[ 9]=mIn->f[ 6];	mTmp.f[13]=mIn->f[ 7];
    mTmp.f[ 2]=mIn->f[ 8];	mTmp.f[ 6]=mIn->f[ 9];	mTmp.f[10]=mIn->f[10];	mTmp.f[14]=mIn->f[11];
    mTmp.f[ 3]=mIn->f[12];	mTmp.f[ 7]=mIn->f[13];	mTmp.f[11]=mIn->f[14];	mTmp.f[15]=mIn->f[15];
    
    *mOut = mTmp;
}



void esMatrixInverse  ( ESMatrix	*mOut, const ESMatrix	*mIn)
{
    static ESMatrix		mDummyMatrix;
    static double		det_1;
    static double		pos, neg, temp;
    
    pos = neg = 0.0;
    temp =  mIn->f[ 0] * mIn->f[ 5] * mIn->f[10];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp =  mIn->f[ 4] * mIn->f[ 9] * mIn->f[ 2];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp =  mIn->f[ 8] * mIn->f[ 1] * mIn->f[ 6];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp = -mIn->f[ 8] * mIn->f[ 5] * mIn->f[ 2];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp = -mIn->f[ 4] * mIn->f[ 1] * mIn->f[10];
    if (temp >= 0.0) pos += temp; else neg += temp;
    temp = -mIn->f[ 0] * mIn->f[ 9] * mIn->f[ 6];
    if (temp >= 0.0) pos += temp; else neg += temp;
    det_1 = pos + neg;
    
    
    if ((det_1 == 0.0) || (_ABS(det_1 / (pos - neg)) < 1.0e-15))
    {
        
        printf("Matrix has no inverse : singular matrix\n");
        return;
    }
    else
    {
        
        det_1 = 1.0 / det_1;
        mDummyMatrix.f[ 0] =   ( mIn->f[ 5] * mIn->f[10] - mIn->f[ 9] * mIn->f[ 6] ) * (float)det_1;
        mDummyMatrix.f[ 1] = - ( mIn->f[ 1] * mIn->f[10] - mIn->f[ 9] * mIn->f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 2] =   ( mIn->f[ 1] * mIn->f[ 6] - mIn->f[ 5] * mIn->f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 4] = - ( mIn->f[ 4] * mIn->f[10] - mIn->f[ 8] * mIn->f[ 6] ) * (float)det_1;
        mDummyMatrix.f[ 5] =   ( mIn->f[ 0] * mIn->f[10] - mIn->f[ 8] * mIn->f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 6] = - ( mIn->f[ 0] * mIn->f[ 6] - mIn->f[ 4] * mIn->f[ 2] ) * (float)det_1;
        mDummyMatrix.f[ 8] =   ( mIn->f[ 4] * mIn->f[ 9] - mIn->f[ 8] * mIn->f[ 5] ) * (float)det_1;
        mDummyMatrix.f[ 9] = - ( mIn->f[ 0] * mIn->f[ 9] - mIn->f[ 8] * mIn->f[ 1] ) * (float)det_1;
        mDummyMatrix.f[10] =   ( mIn->f[ 0] * mIn->f[ 5] - mIn->f[ 4] * mIn->f[ 1] ) * (float)det_1;
        
        
        mDummyMatrix.f[12] = - ( mIn->f[12] * mDummyMatrix.f[ 0] + mIn->f[13] * mDummyMatrix.f[ 4] + mIn->f[14] * mDummyMatrix.f[ 8] );
        mDummyMatrix.f[13] = - ( mIn->f[12] * mDummyMatrix.f[ 1] + mIn->f[13] * mDummyMatrix.f[ 5] + mIn->f[14] * mDummyMatrix.f[ 9] );
        mDummyMatrix.f[14] = - ( mIn->f[12] * mDummyMatrix.f[ 2] + mIn->f[13] * mDummyMatrix.f[ 6] + mIn->f[14] * mDummyMatrix.f[10] );
        
        mDummyMatrix.f[ 3] = 0.0f;
        mDummyMatrix.f[ 7] = 0.0f;
        mDummyMatrix.f[11] = 0.0f;
        mDummyMatrix.f[15] = 1.0f;
    }
    
    
    *mOut = mDummyMatrix;
}
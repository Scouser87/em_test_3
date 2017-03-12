//
//  Render.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef Render_hpp
#define Render_hpp

#include <GL/gl.h>

class Render
{
public:
    static void Translate(float sx, float sy, float sz);
    static void Rotate(float angle, bool rx, bool ry, bool rz);
    static void Scale(float sx, float sy, float sz);
    
    static void GetMatrix(float* cam);
    static void SetMatrix(float* cam);
    
    static void PushMatrix();
    static void PopMatrix();
    static void LoadIdentity();
    
    static void DrawArrays(GLenum mode, GLint first, GLsizei count);
};

#endif /* Render_hpp */

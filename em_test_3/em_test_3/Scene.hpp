//
//  Scene.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <stdio.h>
#include "algebra3.h"
#include "esUtils.hpp"
#include "MouseHandler.hpp"

class Scene
{
public:
    static Scene* s_scene;
    
    Scene(float x, float y);
    
    void LoadShaders();
    void SetUniformMatrix(int& matrixId, GLint dimension, const GLfloat* value);
    void LoadShaderParams();
    
    void Start();
    
    void CountMatrix();
    void Draw();
    
private:
    vec2            m_size;
    
    vec2            m_rectPos;
    float           m_angle;
    float           m_scale;
    float           m_scaleInc;
    
    //shaders
    unsigned int    m_program;
    unsigned int    m_positionAttrib;
    unsigned int    m_textCoordAttrib;
    int             m_uniformProjection;
    int             m_uniformModelView;
    int             m_uniformColor;
    
    ESMatrix        m_projectionMatrix;
    ESMatrix        m_modelMatrix;
    float           m_color[4];
    float           m_vertex[12];
    float           m_textCoord[12];
    GLuint          m_vbo;
    GLuint          m_vboTexCoord;
    
    MouseHandler*   m_mouseHandler;
};

#endif /* Scene_hpp */

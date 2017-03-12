//
//  Scene.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "Scene.hpp"
#include <iostream>
#include <unistd.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include "FileLoader.hpp"
#include "Trace.hpp"

Scene* Scene::s_scene = NULL;

Scene::Scene(float x, float y)
{
    Scene::s_scene = this;
    m_size.x = x;
    m_size.y = y;
    
    emscripten_set_canvas_size(m_size.x, m_size.y);
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.preferLowPowerToHighPerformance = attr.failIfMajorPerformanceCaveat = 0;
    attr.enableExtensionsByDefault = 1;
    attr.premultipliedAlpha = 0;
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(0, &attr);
    emscripten_webgl_make_context_current(ctx);
    
    LoadShaders();
    LoadShaderParams();
    
    m_mouseHandler = new MouseHandler;
}

void Scene::LoadShaders()
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    std::string v_shader = gReadFile("resources/shaders/DefaultTexture.vsh", "rb");
    const char* v_shaderCh = v_shader.c_str();
    glShaderSource(vs, 1, &v_shaderCh, 0);
    glCompileShader(vs);
    GLint isCompiled = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength = 0;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &maxLength);
        char *buf = new char[maxLength];
        glGetShaderInfoLog(vs, maxLength, &maxLength, buf);
        printf("%s\n", buf);
    }
    
    GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
    std::string f_shader = gReadFile("resources/shaders/DefaultTexture.fsh", "rb");
    const char* f_shaderCh = f_shader.c_str();
    glShaderSource(ps, 1, &f_shaderCh, 0);
    glCompileShader(ps);
    glGetShaderiv(ps, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength = 0;
        glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &maxLength);
        char *buf = new char[maxLength];
        glGetShaderInfoLog(ps, maxLength, &maxLength, buf);
        printf("%s\n", buf);
    }
    
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, ps);
    
//    m_positionAttrib = 0;
//    glBindAttribLocation(m_program, m_positionAttrib, "Position");
    
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &isCompiled);
    if (!isCompiled)
    {
        GLint maxLength = 0;
        glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);
        char *buf = new char[maxLength];
        glGetProgramInfoLog(m_program, maxLength, &maxLength, buf);
        printf("%s\n", buf);
    }
    
    m_uniformProjection = glGetUniformLocation(m_program, "Projection");
    m_uniformModelView = glGetUniformLocation(m_program, "Modelview");
//    m_uniformColor = glGetUniformLocation(m_program, "Color");
    m_positionAttrib = glGetAttribLocation(m_program, "Position");
    m_textCoordAttrib = glGetAttribLocation(m_program, "TexCoord");
    
    glDeleteShader(vs);
    glDeleteShader(ps);
}

void Scene::SetUniformMatrix(int& matrixId, GLint dimension, const GLfloat* value)
{
    glUniformMatrix4fv(matrixId, 1, GL_FALSE, value);
}

void Scene::LoadShaderParams()
{
    esMatrixLoadIdentity(&m_projectionMatrix);
    esMatrixLoadIdentity(&m_modelMatrix);
    
    esOrtho(&m_projectionMatrix, 0, 1024, 0, 768,-1, 1);
    
    memset(m_color, 0, sizeof(float)*4);
    m_color[0] = 1;
    
#define RECT_WIDTH 50
    
    memset(m_vertex, 0, sizeof(float)*12);
    memset(m_textCoord, 0, sizeof(float)*12);
    m_vertex[0] = -RECT_WIDTH; m_vertex[1] = -RECT_WIDTH;
    m_vertex[2] = RECT_WIDTH; m_vertex[3] = -RECT_WIDTH;
    m_vertex[4] = RECT_WIDTH; m_vertex[5] = RECT_WIDTH;
    m_vertex[6] = RECT_WIDTH; m_vertex[7] = RECT_WIDTH;
    m_vertex[8] = -RECT_WIDTH; m_vertex[9] = RECT_WIDTH;
    m_vertex[10] = -RECT_WIDTH; m_vertex[11] = -RECT_WIDTH;
    
    
    
    m_rectPos = vec2();
    m_angle = 0;
    m_scale = 1.0f;
    m_scaleInc = 0.01f;
    
    glUseProgram(m_program);
    
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
    glGenBuffers(1, &m_vboTexCoord);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoord);
    
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    int texture, width, height;
    texture = png_texture_load("resources/2.png", width, height);
    if (texture)
        glBindTexture(GL_TEXTURE_2D, texture);
    
    float minS = 0.0f;
    float minT = 0.0f;
    float maxS = 512.0f/512.0f;
    float maxT = 512.0f/512.0f;
    
    m_textCoord[0] = minS; m_textCoord[1] = minT;
    m_textCoord[2] = maxS; m_textCoord[3] = minT;
    m_textCoord[4] = maxS; m_textCoord[5] = maxT;
    m_textCoord[6] = maxS; m_textCoord[7] = maxT;
    m_textCoord[8] = minS; m_textCoord[9] = maxT;
    m_textCoord[10] = minS; m_textCoord[11] = minT;
    
    SetUniformMatrix(m_uniformProjection, 1, (float*)&m_projectionMatrix);
    SetUniformMatrix(m_uniformModelView, 1, (float*)&m_modelMatrix);
    glUniform4fv(m_uniformColor, 1, m_color);
}

void Scene_MainLoop()
{
    Scene::s_scene->Draw();
}

void Scene::Start()
{
    emscripten_set_main_loop(Scene_MainLoop, 60, 0);
}

void Scene::CountMatrix()
{
    esMatrixLoadIdentity(&m_modelMatrix);
    esTranslate(&m_modelMatrix, m_rectPos.x, m_rectPos.y, 0);
    esRotate(&m_modelMatrix, m_angle, 0, 0, 1);
    esScale(&m_modelMatrix, m_scale, m_scale, 0.0f);
    
    SetUniformMatrix(m_uniformModelView, 1, (float*)&m_modelMatrix);
}

void Scene::Draw()
{
//    m_rectPos += vec2(1,1);
    m_rectPos.x = m_mouseHandler->GetCanvasPos().x;
    m_rectPos.y = m_size.y - m_mouseHandler->GetCanvasPos().y;
    
    m_angle += 2;
    if (m_angle >= 360)
        m_angle = 0;
    m_scale += m_scaleInc;
    if (m_scale >= 1.5f)
    {
        m_scale = 1.5;
        m_scaleInc = -m_scaleInc;
    }
    else if(m_scale <= 0.5f)
    {
        m_scale = 0.5;
        m_scaleInc = -m_scaleInc;
    }
    CountMatrix();
    
    glClearColor(0.5,0.5,0.5,1);
    glClear(GL_COLOR_BUFFER_BIT);

//    glEnableVertexAttribArray(m_positionAttrib);
//    glVertexAttribPointer(m_positionAttrib, 2, GL_FLOAT, 0, 0, m_vertex);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex), m_vertex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(m_positionAttrib);
    glVertexAttribPointer(m_positionAttrib, 2, GL_FLOAT, 0, sizeof(float)*2, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_textCoord), m_textCoord, GL_STATIC_DRAW);
    glEnableVertexAttribArray(m_textCoordAttrib);
    glVertexAttribPointer(m_textCoordAttrib, 2, GL_FLOAT, 0, sizeof(float)*2, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
//
//  ShaderManager.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef ShaderManager_hpp
#define ShaderManager_hpp

#include <string>
#include <map>
#include <stack>
#include "esUtils.hpp"
#include "pugixml.hpp"

class CShader
{
public:
    CShader(pugi::xml_node node);
    
    void PrintInfo();
    
    int GetUniform(std::string ID);
    int GetAttribute(std::string ID);
    
    void SetUniform(std::string ID, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void SetUniformMatrix(std::string ID, GLint dimension, const GLfloat* value);
    void SetAttribute(std::string ID, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr, int arraySize);
    
    void ActivateProgram();
    
    void SetProjectionMatrix(float*cam);
    void SetModelviewMatrix(float*cam);
    
    const std::string& GetName() { return m_name; }
    
private:
    std::string     m_name;
    
    unsigned int    m_program;
    
    std::map<std::string, int> m_uniforms;
    std::map<std::string, int> m_attributes;
    std::map<std::string, int> m_attributeBuffers;    
};

class CShaderManager
{
    static CShaderManager* s_shaderMngr;
public:
    static CShaderManager* Get();
    
    CShaderManager();
    ~CShaderManager();
    
    void Init(float w, float h);
    void LoadShaders();
    
    void SetActiveShader(std::string name);
    CShader* GetActiveShader() { return m_activeShader; }
    
    void LoadIdentity();
    void Translate(float sx, float sy, float sz);
    void Rotate(float angle, bool rx, bool ry, bool rz);
    void Scale(float sx, float sy, float sz);
    
    void BindProjection(bool immediately=false); // биндует проекционную матрицу в активный шейдер
    void BindModelview(bool immediately=false); // биндует модельную матрицу в активный шейдер
    
    void GetProjectionMatrix(float* cam);
    void GetModelviewMatrix(float* cam);
    void SetModelviewMatrix(float* cam);
    void SetModelviewMatrix(const float* cam);
    
    void PushMatrix();
    void PopMatrix();
    
private:
    void LoadShader(std::string path);
    
    std::map<std::string, CShader*> m_shaders;
    CShader* m_activeShader;
    
    ESMatrix        m_projectionMatrix;
    ESMatrix        m_modelMatrix;
    
    std::stack<ESMatrix> m_modelviewStack; // стек для матрица преобразований 
};

#endif /* ShaderManager_hpp */

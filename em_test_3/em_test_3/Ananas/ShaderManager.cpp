//
//  ShaderManager.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "ShaderManager.hpp"

#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include "esUtils.hpp"
#include "FileLoader.hpp"
#include "Trace.hpp"

CShader::CShader(pugi::xml_node node)
{
    m_name = node.attribute("name").as_string();
    
    pugi::xml_node vertexShader = node.child("vertexShader");
    pugi::xml_node fragmentShader = node.child("fragmentShader");
        
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    std::string v_shader = gReadFile(vertexShader.attribute("path").as_string(), "rb");
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
    std::string f_shader = gReadFile(fragmentShader.attribute("path").as_string(), "rb");
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
    
    glUseProgram(m_program);
    
    pugi::xml_node uniform = node.child("uniforms").first_child();
    while (uniform)
    {
        std::string _id = uniform.attribute("id").as_string();
        m_uniforms[_id] = glGetUniformLocation(m_program, _id.c_str());
        uniform = uniform.next_sibling();
    }
    
    pugi::xml_node attribute = node.child("attributes").first_child();
    while (attribute)
    {
        std::string _id = attribute.attribute("id").as_string();
        m_attributes[_id] = glGetAttribLocation(m_program, _id.c_str());
        attribute = attribute.next_sibling();
    }
    
    auto itAt = m_attributes.begin();
    while (itAt != m_attributes.end())
    {
        GLuint attributeBufferId = 0;
        
        glGenBuffers(1, &attributeBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, attributeBufferId);
        
        m_attributeBuffers[itAt->first] = attributeBufferId;
        
        ++itAt;
    }
    
//    PrintInfo();
    
    glDeleteShader(vs);
    glDeleteShader(ps);
}

void CShader::PrintInfo()
{
    LOG("Shader %s info:", m_name.c_str());
    auto it = m_uniforms.begin();
    while (it != m_uniforms.end())
    {
        LOG("uniform %s - %d", it->first.c_str(), it->second);
        ++it;
    }
    auto itAt = m_attributes.begin();
    while (itAt != m_attributes.end())
    {
        LOG("attribute %s - %d", itAt->first.c_str(), itAt->second);
        ++itAt;
    }
}

int CShader::GetUniform(std::string ID)
{
    auto itr = m_uniforms.find(ID);
    if(itr != m_uniforms.end())
        return itr->second;
    return -1;
}

int CShader::GetAttribute(std::string ID)
{
    auto itr = m_attributes.find(ID);
    if(itr != m_attributes.end())
        return itr->second;
    return -1;
}

void CShader::SetUniformMatrix(std::string ID, GLint dimension, const GLfloat* value)
{
    int uniform = GetUniform(ID);
    if(uniform == -1)
    {
        ALOG("Uniform %s not exist\n", ID.c_str());
        return;
    }
    switch (dimension) {
        case 2:
            glUniformMatrix2fv(uniform, 1, GL_FALSE, value);
            break;
        case 3:
            glUniformMatrix3fv(uniform, 1, GL_FALSE, value);
            break;
        case 4:
            glUniformMatrix4fv(uniform, 1, GL_FALSE, value);
            break;
        default:
            ALOG("ERROR: current dimension(%d) for SetUniformMatrix not supported\n", dimension);
            break;
    }
}

void CShader::SetUniform(std::string ID, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    int uniform = GetUniform(ID);
    if(uniform == -1)
    {
        ALOG("Uniform %s not exist\n", ID.c_str());
        return;
    }
    glUniform4f(uniform, x, y, z, w);
}

void CShader::SetAttribute(std::string ID, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr, int arraySize)
{
    int attribute = GetAttribute(ID);
    if(attribute == -1)
    {
        ALOG("Attribute %s not exist\n", ID.c_str());
        return;
    }
//    glVertexAttribPointer(attribute, size, type, normalized, stride, ptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_attributeBuffers[ID]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*arraySize, ptr, GL_STATIC_DRAW);
    glEnableVertexAttribArray(attribute);
    glVertexAttribPointer(attribute, size, type, normalized, sizeof(float)*2, 0);
}

void CShader::ActivateProgram()
{
    glUseProgram(m_program);
}

void CShader::SetProjectionMatrix(float*cam)
{
    SetUniformMatrix("Projection", 4, cam);
}

void CShader::SetModelviewMatrix(float*cam)
{
    SetUniformMatrix("Modelview", 4, cam);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////      CShaderManager        //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

CShaderManager* CShaderManager::s_shaderMngr = NULL;

CShaderManager* CShaderManager::Get()
{
    return s_shaderMngr;
}

CShaderManager::CShaderManager()
:m_activeShader(NULL)
{
    CShaderManager::s_shaderMngr = this;    
}

CShaderManager::~CShaderManager()
{
    CShaderManager::s_shaderMngr = NULL;
    auto it = m_shaders.begin();
    while (it != m_shaders.end())
    {
        delete it->second;
        ++it;
    }
    m_shaders.clear();
}

void CShaderManager::Init(float w, float h)
{
    esMatrixLoadIdentity(&m_projectionMatrix);
    esMatrixLoadIdentity(&m_modelMatrix);
    
    esOrtho(&m_projectionMatrix, 0, w, 0, h,-1, 1);
}

void CShaderManager::LoadShaders()
{
    LoadShader("resources/shaders/Shaders.xml");
}

void CShaderManager::SetActiveShader(std::string name)
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end())
    {
        m_activeShader = it->second;
        m_activeShader->ActivateProgram();
    }
}

void CShaderManager::LoadIdentity()
{
    esMatrixLoadIdentity(&m_modelMatrix);
    BindModelview();
}

void CShaderManager::Translate(float sx, float sy, float sz)
{
   	esTranslate(&m_modelMatrix, sx, sy, sz);
    BindModelview();
}

void CShaderManager::Rotate(float angle, bool rx, bool ry, bool rz)
{
    esRotate(&m_modelMatrix, -angle, rx, ry, rz);
    BindModelview();
}

void CShaderManager::Scale(float sx, float sy, float sz)
{
    esScale(&m_modelMatrix, sx, sy, sz);
    BindModelview();
}

void CShaderManager::BindProjection(bool immediately)
{
    if(m_activeShader != NULL)
        m_activeShader->SetProjectionMatrix((float*)&m_projectionMatrix);
}

void CShaderManager::BindModelview(bool immediately)
{
    if(m_activeShader != NULL)
        m_activeShader->SetModelviewMatrix((float*)&m_modelMatrix);
}

void CShaderManager::GetProjectionMatrix(float* cam)
{
    memcpy(cam, m_projectionMatrix.f, sizeof(float)*16);
}

void CShaderManager::GetModelviewMatrix(float* cam)
{
    memcpy(cam, m_modelMatrix.f, sizeof(float)*16);
}

void CShaderManager::SetModelviewMatrix(float* cam)
{
    memcpy(m_modelMatrix.f, cam, sizeof(float)*16);
    BindModelview();
}

void CShaderManager::SetModelviewMatrix(const float* cam)
{
    memcpy(m_modelMatrix.f, cam, sizeof(float)*16);
    BindModelview();
}

void CShaderManager::PushMatrix()
{
    m_modelviewStack.push(m_modelMatrix);
}

void CShaderManager::PopMatrix()
{
    if (m_modelviewStack.empty())
        return;
    m_modelMatrix = m_modelviewStack.top();
    m_modelviewStack.pop();
    BindModelview();
}

void CShaderManager::LoadShader(std::string path)
{
    pugi::xml_document doc;
    std::string fileContent = gReadFile(path.c_str(), "rb");
    pugi::xml_parse_result result = doc.load(fileContent.c_str());
    
    if (result.status != pugi::status_ok)
    {
        ALOG("CShader - file  not found %s\n", path.c_str());
        return;
    }
    
    pugi::xml_node nodeShader = doc.child("Shaders").first_child();
    while (nodeShader)
    {
        CShader* shader = new CShader(nodeShader);
        m_shaders[shader->GetName()] = shader;
        nodeShader = nodeShader.next_sibling();
    }    
}



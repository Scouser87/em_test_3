//
//  Render.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "Render.hpp"
#include "ShaderManager.hpp"

void Render::Translate(float sx, float sy, float sz)
{
    CShaderManager::Get()->Translate(sx, sy, sz);
}

void Render::Rotate(float angle, bool rx, bool ry, bool rz)
{
    CShaderManager::Get()->Rotate(angle, rx, ry, rz);
}

void Render::Scale(float sx, float sy, float sz)
{
    CShaderManager::Get()->Scale(sx, sy, sz);
}

void Render::GetMatrix(float* cam)
{
    CShaderManager::Get()->GetModelviewMatrix(cam);
}

void Render::SetMatrix(float* cam)
{
    CShaderManager::Get()->SetModelviewMatrix(cam);
}

void Render::PushMatrix()
{
    CShaderManager::Get()->PushMatrix();
}

void Render::PopMatrix()
{
    CShaderManager::Get()->PopMatrix();
}

void Render::LoadIdentity()
{
    CShaderManager::Get()->LoadIdentity();
}

void Render::DrawArrays(GLenum mode, GLint first, GLsizei count)
{
    CShaderManager::Get()->BindProjection(true);
    CShaderManager::Get()->BindModelview(true);
    glDrawArrays(mode, first, count);
}
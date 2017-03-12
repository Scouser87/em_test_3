//
//  CImage.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "CImage.hpp"
#include <GL/gl.h>
#include "FileLoader.hpp"

CImage::CImage(std::string path)
{
    m_id = png_texture_load(path.c_str(), m_width, m_height);
    if (m_id)
        glBindTexture(GL_TEXTURE_2D, m_id);
    
    minS = 0.0f;
    minT = 0.0f;
    maxS = 1.0f;
    maxT = 1.0f;
}

void CImage::BindTexture()
{
    static GLint textId = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &textId);
    if(textId != m_id)
        glBindTexture(GL_TEXTURE_2D, m_id);
}
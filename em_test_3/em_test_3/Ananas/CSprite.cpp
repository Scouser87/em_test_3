/*
 *  CSprite.cpp
 *  Ananas
 *
 *  Created by Anton Zagovalko on 24.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved. 
 *
 */

#include "CSprite.h"
#include "CDirector.h"
#include <algorithm>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include "Render.hpp"
#include "ShaderManager.hpp"
#include "Trace.hpp"

#define ENABLE_PROFILING

void CSprite::SetVertex()
{
	if (m_size.x != 0 || m_size.y != 0)
	{
		if (!m_vertex) m_vertex = new float[8];

		m_vertex[0] = - m_size.x*m_anchorPoint.x;		m_vertex[1] = - m_size.y*m_anchorPoint.y;
		m_vertex[2] = - m_size.x*m_anchorPoint.x;		m_vertex[3] = + m_size.y*(1-m_anchorPoint.y);
		m_vertex[4] = + m_size.x*(1-m_anchorPoint.x);	m_vertex[5] = - m_size.y*m_anchorPoint.y;
		m_vertex[6] = + m_size.x*(1-m_anchorPoint.x);	m_vertex[7] = + m_size.y*(1-m_anchorPoint.y);
		
/*		printf("\n");
		for (int i = 0; i < 4; i++)
		{
			printf("m_vertex[%i] x = %f  y = %f\n", i, m_vertex[i*2], m_vertex[i*2+1]);
		}
*/		
	}
}

void CSprite::SetTexCoord(const vec2& p, const vec2& to)
{
	if (m_Texture)
	{
		if (!m_textCoord) m_textCoord = new float[8];
        
        m_textureCoordFrom = p;
        m_textureCoordTo = to;

		m_textCoord[0] = p.x  * m_Texture->GetMaxS();        m_textCoord[1] = p.y * m_Texture->GetMaxT();
		m_textCoord[2] = p.x  * m_Texture->GetMaxS();        m_textCoord[3] = to.y  * m_Texture->GetMaxT();
		m_textCoord[4] = to.x * m_Texture->GetMaxS();        m_textCoord[5] = p.y * m_Texture->GetMaxT();
		m_textCoord[6] = to.x * m_Texture->GetMaxS();        m_textCoord[7] = to.y  * m_Texture->GetMaxT();
	}
}

void CSprite::CountMatrix()
{
    if (m_pos != vec2(0, 0))
        Render::Translate(m_pos.x, m_pos.y, 0);
    if (m_angle != 0)
        Render::Rotate(m_angle*180/M_PI, 0, 0, 1);
    if (m_scale != vec2(1, 1))
        Render::Scale(m_scale.x, m_scale.y, 1);
    if (m_flip == efHorizontal || m_flip == efBoth)  Render::Scale(-1, 1, 1);
    if (m_flip == efVertical   || m_flip == efBoth)  Render::Scale(1, -1, 1);
    
    
}

void CSprite::ResetMatrix()
{
	m_isMatrixSet = false;
	
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* s = dynamic_cast< CSprite* > (*i);
        if (s) s->ResetMatrix();
    }
}

void CSprite::SetMatrix()
{
    if (!m_isMatrixSet)
    {
        CountMatrix();
        
        if (!m_matrixSave) m_matrixSave = new float[16];
        Render::GetMatrix(m_matrixSave);
        m_isMatrixSet = true;
    }
    else 
    {
        Render::SetMatrix(m_matrixSave);
    }
}

void CSprite::SetMatrixRecur()
{
	Render::PushMatrix();
	SetMatrix();
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* s = dynamic_cast< CSprite* > (*i);
        if (s) s->SetMatrixRecur();
    }
    
	Render::PopMatrix();
}

CSprite::CSprite()
:m_Texture(		NULL),
m_isPremultipliedAlpha(false),
m_pos(			0, 0),	
m_size(			0, 0),
m_scale(		1, 1),
m_anchorPoint(	0.5, 0.5),
m_angle(		0),
m_flip(			efNone),
m_color(		1, 1, 1, 0),
m_blendColor(   EBC_Multiply),
m_opacity(		1),
m_opacityWithParent(1),
m_isVisible(	true),
m_vertex(		NULL),
m_textCoord(	NULL),
m_matrixSave(	NULL),
m_isMatrixSet(	false),
m_blendMode(	ebmNormal),
m_drawAfterSprite(NULL),
m_isHUD(		false),
m_isScissor(	false),
m_isSaveScissor(false),
m_saveScissor(  NULL),
m_blendParams(),
m_texCoordRotated(false),
m_fbo(NULL),
m_fboID(-1),
m_isNotCalculationToFBO(false),
m_isRenderToFBO(false),
m_isOnlyGenerateFBO(false),
m_enableRenderProfiling(false),
m_needRecalcOpacityWithParent(false),
m_changeOpacityForChildren(true),
m_textureCoordFrom(vec2(0, 0)),
m_textureCoordTo(vec2(1, 1))
{
}

CSprite::CSprite(const std::string& imagePath)
:
m_pos(			0, 0),	
m_scale(		1, 1),
m_anchorPoint(	0.5, 0.5),
m_angle(		0),
m_flip(			efNone),
m_blendColor(   EBC_Multiply),
m_opacity(		1),
m_opacityWithParent(1),
m_isVisible(	true),
m_vertex(		NULL),
m_textCoord(	NULL),
m_matrixSave(	NULL),
m_isMatrixSet(	false),
m_blendMode(	ebmNormal),
m_drawAfterSprite(NULL),
m_isHUD(		false),
m_isScissor(	false),
m_isSaveScissor(false),
m_saveScissor(  NULL),
m_blendParams(),
m_texCoordRotated(false),
m_fbo(NULL),
m_fboID(-1),
m_isNotCalculationToFBO(false),
m_isRenderToFBO(false),
m_isOnlyGenerateFBO(false),
m_enableRenderProfiling(false),
m_needRecalcOpacityWithParent(false),
m_changeOpacityForChildren(true),
m_textureCoordFrom(vec2(0, 0)),
m_textureCoordTo(vec2(1, 1)),
m_isPremultipliedAlpha(false)
{
    SetImage(imagePath);
}

CSprite::CSprite(CImage* image)
:
m_pos(			0, 0),	
m_scale(		1, 1),
m_anchorPoint(	0.5, 0.5),
m_angle(		0),
m_flip(			efNone),
m_blendColor(   EBC_Multiply),
m_opacity(		1),
m_opacityWithParent(1),
m_isVisible(	true),
m_vertex(		NULL),
m_textCoord(	NULL),
m_matrixSave(	NULL),
m_isMatrixSet(	false),
m_blendMode(	ebmNormal),
m_drawAfterSprite(NULL),
m_isHUD(		false),
m_isScissor(	false),
m_isSaveScissor(false),
m_saveScissor(  NULL),
m_blendParams(),
m_texCoordRotated(false),
m_fbo(NULL),
m_fboID(-1),
m_isNotCalculationToFBO(false),
m_isRenderToFBO(false),
m_isOnlyGenerateFBO(false),
m_enableRenderProfiling(false),
m_needRecalcOpacityWithParent(false),
m_changeOpacityForChildren(true),
m_textureCoordFrom(vec2(0, 0)),
m_textureCoordTo(vec2(1, 1)),
m_isPremultipliedAlpha(false)
{
    SetImage(image);	
}

CSprite::CSprite(const sColor& color)
:
m_Texture(		NULL),
m_pos(			0, 0),	
m_size(			CDirector::GetDirector().GetCanvasSize()),
m_scale(		1, 1),
m_anchorPoint(	0, 0),
m_angle(		0),
m_flip(			efNone),
m_color(		color),
m_blendColor(   EBC_Multiply),
m_opacity(		1),
m_opacityWithParent(1),
m_isVisible(	true),
m_vertex(		NULL),
m_textCoord(	NULL),
m_matrixSave(	NULL),
m_isMatrixSet(	false),
m_blendMode(	ebmNormal),
m_drawAfterSprite(NULL),
m_isHUD(		false),
m_isScissor(	false),
m_isSaveScissor(false),
m_saveScissor(  NULL),
m_blendParams(),
m_texCoordRotated(false),
m_fbo(NULL),
m_fboID(-1),
m_isNotCalculationToFBO(false),
m_isRenderToFBO(false),
m_isOnlyGenerateFBO(false),
m_enableRenderProfiling(false),
m_needRecalcOpacityWithParent(false),
m_changeOpacityForChildren(true),
m_textureCoordFrom(vec2(0, 0)),
m_textureCoordTo(vec2(1, 1)),
m_isPremultipliedAlpha(false)
{
	SetVertex();
}

CSprite::CSprite(const CSprite& sprite)
:CNode(sprite),
m_Texture(		sprite.m_Texture),
m_pos(			sprite.m_pos),	
m_size(			sprite.m_size),
m_scale(		sprite.m_scale),
m_anchorPoint(	sprite.m_anchorPoint),
m_angle(		sprite.m_angle),
m_flip(			sprite.m_flip),
m_color(		sprite.m_color),
m_blendColor(   sprite.m_blendColor),
m_opacity(		sprite.m_opacity),
m_opacityWithParent(1),
m_isVisible(	sprite.m_isVisible),
m_vertex(       NULL),
m_textCoord(    NULL),
m_matrixSave(	NULL),
m_isMatrixSet(	sprite.m_isMatrixSet),
m_blendMode(	sprite.m_blendMode),
m_drawAfterSprite(NULL),
m_isHUD(		sprite.m_isHUD),
m_isScissor(	sprite.m_isScissor),
m_isSaveScissor(sprite.m_isSaveScissor),
m_saveScissor(  NULL),
m_blendParams(  sprite.m_blendParams),
m_texCoordRotated(sprite.m_texCoordRotated),
m_fbo(NULL),
m_fboID(sprite.m_fboID),
m_isNotCalculationToFBO(false),
m_isRenderToFBO(false),
m_isOnlyGenerateFBO(sprite.m_isOnlyGenerateFBO),
m_enableRenderProfiling(false),
m_needRecalcOpacityWithParent(true),
m_changeOpacityForChildren(sprite.m_changeOpacityForChildren),
m_textureCoordFrom(sprite.m_textureCoordFrom),
m_textureCoordTo(sprite.m_textureCoordTo),
m_isPremultipliedAlpha(sprite.m_isPremultipliedAlpha)
{
    if (sprite.m_vertex)
    {
        m_vertex = new float[8];
        memcpy(m_vertex, sprite.m_vertex, sizeof(float)*8);
    }
    if (sprite.m_textCoord)
    {
        m_textCoord = new float[8];
        memcpy(m_textCoord, sprite.m_textCoord, sizeof(float)*8);
    }
    if (m_isSaveScissor && sprite.m_saveScissor)
    {
        m_saveScissor = new int[4];
        memcpy(m_saveScissor, sprite.m_saveScissor, sizeof(int)*4);
    }
	if (m_isMatrixSet && sprite.m_matrixSave)
    {
        m_matrixSave = new float[16];
        memcpy(m_matrixSave, sprite.m_matrixSave, sizeof(float)*16);
    }
//    if(sprite.m_fbo)
//        m_fbo = CDirector::GetFBOManager().AddFBO(m_fboID, this);
}

CSprite::~CSprite()
{
	DontDrawAfter();
	RemoveAllDrawAfter();

//	if (m_Texture && dynamic_cast<CImageSprite*>(m_Texture))
//	{
//		CDirector::GetImageManager().RemoveImage(m_Texture);
//	}
	if (m_vertex)
		delete[] m_vertex;
	if (m_textCoord) 
		delete[] m_textCoord;
	if (m_matrixSave) 
		delete[] m_matrixSave;
    
    if (m_saveScissor)
		delete[] m_saveScissor;
    
//    if(m_fbo)
//        CDirector::GetFBOManager().RemoveFBO(m_fboID, this);
    
    if (m_Texture)
    {
        delete m_Texture;
        m_Texture = NULL;
    }
}

void CSprite::RecalcOpacityWithParent()
{
    m_opacityWithParent = m_opacity;
    CNode* sp = this;
    while (sp->GetParent()) {
        sp = sp->GetParent();
        CSprite* sprite = dynamic_cast<CSprite*>(sp);
        if(sprite && sprite->GetChangeChildrenOpacity())
            m_opacityWithParent *= sprite->GetOpacity();
    }

}

void CSprite::Draw()
{
    if(m_needRecalcOpacityWithParent)
    {
        m_needRecalcOpacityWithParent = false;
        RecalcOpacityWithParent();
    }
	if (m_vertex && m_opacity != 0 && m_color.sAlpha != 0 && m_opacityWithParent != 0)
	{
/*		if (CCamera::GetCamera().GetCheckSprites() && !GetIsHUDThisOrParent())
		{
			if (!IntersectWithCamera())
				return;
		}*/
//
#if OS != OS_QT_VIEWER
        if (!IntersectWithCamera()&&!m_isRenderToFBO)
            return;
#endif
        
        static int _sFactorRGB = GL_SRC_ALPHA, _sFactorAlpha = GL_SRC_ALPHA, _dFactorRGB = GL_ONE_MINUS_SRC_ALPHA, _dFactorAlpha = GL_ONE_MINUS_SRC_ALPHA;
        glGetIntegerv(GL_BLEND_SRC_RGB, &_sFactorRGB);
        glGetIntegerv(GL_BLEND_DST_RGB, &_dFactorRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &_sFactorAlpha);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &_dFactorAlpha);
        
        if(m_blendMode == ebmNormal && !m_isPremultipliedAlpha && m_isRenderToFBO)
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
        else
            glBlendFuncSeparate(m_blendParams.srcRGB, m_blendParams.dstRGB, m_blendParams.srcAlpha, m_blendParams.dstAlpha);
        
        float alpha = m_color.sAlpha * m_opacityWithParent;
        float multiplier = m_isPremultipliedAlpha ? alpha : 1;
        sColor color(m_color.sRed * multiplier, m_color.sGreen * multiplier, m_color.sBlue * multiplier, alpha);
        
        if (m_Texture)
		{
            CShaderManager::Get()->SetActiveShader("DefaultTexture");
            
			m_Texture->BindTexture();
            CShaderManager::Get()->GetActiveShader()->SetAttribute("TexCoord", 2, GL_FLOAT, 0, 0, m_textCoord, 8);
            
		}
		else
		{
			CShaderManager::Get()->SetActiveShader("DefaultColor");
            CShaderManager::Get()->GetActiveShader()->SetUniform("Color", color.sRed, color.sGreen, color.sBlue, color.sAlpha);
		}

        CShaderManager::Get()->GetActiveShader()->SetAttribute("Position", 2, GL_FLOAT, 0, 0, m_vertex, 8);
        Render::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glBlendFuncSeparate(_sFactorRGB, _dFactorRGB, _sFactorAlpha, _dFactorAlpha);
	}
}

void CSprite::Simulate(float dt)
{
//    if(m_fbo != NULL)
//    {
//        bool isNotCalculationToFBO(!m_fbo->UpdateTransform(this));
//        if ( isNotCalculationToFBO != m_isNotCalculationToFBO)
//            SetNotCalculationToFBO(isNotCalculationToFBO);
//    }
    
    CNode::Simulate(dt);
}

void CSprite::Visit()
{
    
//#ifdef ENABLE_PROFILING
//    if(m_enableRenderProfiling)
//        m_profilingTimer.Reset();
//#endif

    
//    if(m_fbo != NULL && !m_isRenderToFBO)
//    {
//        m_fbo->UpdateTexture(this);
//        if(!m_isOnlyGenerateFBO)
//        {
//            Render::PushMatrix();
//            Render::Translate(m_pos.x, m_pos.y, 0);
//            
//            m_fbo->Draw();
//            
//            Render::PopMatrix();
//        }
//        return;
//    }
    
    if (IsDelete()) 
        return;


	if (IsVisible())
	{
		Render::PushMatrix();
		SetMatrix();
		
        if (!m_drawAfterSprite)		// если данный элемент не рисуется после какогото
            Render();

        Render::PopMatrix();
	}
		DrawAfterRun();

//#ifdef ENABLE_PROFILING
//    if(m_enableRenderProfiling)
//        printf("Profiling: execution time for render %s is %f sec\n", m_profilingName.c_str(), m_profilingTimer.GetTimeD());
//#endif

}

void CSprite::DrawAfterRun()	// рисование данного элемента после какогото
{
    if (m_drawAfterSpriteList.empty() || m_drawAfterSpriteList.size() == 0)
        return;
  
	std::list< CSprite* >::iterator i = m_drawAfterSpriteList.begin();
	for (; i != m_drawAfterSpriteList.end(); i++)
	{
        CSprite* sprt = (*i);
        if (sprt && sprt->IsVisible())
        {
            if (sprt->GetParent() == NULL)
                ALOG("Error CSprite::DrawAfterRun : parent == NULL");
            
            Render::PushMatrix();
            CShaderManager::Get()->LoadIdentity();
            if (sprt->IsMatrixSet())
                Render::SetMatrix((float*)sprt->GetMatrixSave());
            else
            {
                std::vector<CSprite*> parents;
                CSprite* parent = dynamic_cast<CSprite*>(sprt->GetParent());
                while (parent)
                {
                    parents.insert(parents.begin(), parent);
                    parent = dynamic_cast<CSprite*>(parent->GetParent());
                }
                for (int j = 0; j < parents.size(); j++)
                    parents[j]->SetMatrix();
                sprt->SetMatrix();
            }
            
            sprt->Render();
            
            Render::PopMatrix();
        }
	}	
}

void CSprite::Render()
{
    
    bool isRotatedScissor(false);
    if (m_isScissor)
    {
        isRotatedScissor = GetGlobalFromLocalCoord(vec2(0,10)).y != GetGlobalFromLocalCoord(vec2(10,10)).y;
        EnableScissor(isRotatedScissor);
    }
    
//    if (m_isHUD)
//        CCamera::GetCamera().SetHud(true);
    
    if (!BeforeVisit())
        Draw();
    
    std::list< CNode* >::iterator j = GetChildren().begin();
    for (; j != GetChildren().end(); j++)
    {
        (*j)->Visit();
    }
    
    if (m_isScissor)
    {
        DisableScissor(isRotatedScissor);
    }
//    if (m_isHUD) CCamera::GetCamera().SetHud(false);
    
    AfterVisit();
}


CSprite* CSprite::SetVisible(const bool& visible)
{
	m_isVisible = visible;
	return this;
}

CSprite* CSprite::SetVisibleAll(const bool& visible)
{
    m_isVisible = visible;
	
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* spr = dynamic_cast<CSprite*>(*i);
        if (spr)
            spr->SetVisibleAll(visible);
    }
	
    return this;
}

CSprite* CSprite::SetActive(const bool& isActive)
{
    SetVisible(isActive);
    SetPause(!isActive);
    //SetIsTouchesOn(isActive);
    return this;
}

CSprite* CSprite::SetIsHUD(bool isHUD)
{
	m_isHUD = isHUD;
	
/*	std::list< CNode* >::iterator i = GetChildren().begin();
	for (; i != GetChildren().end(); i++)
	{
		((CSprite*)(*i))->SetIsHUD(isHUD);
	}
 */
    return this;
}

bool CSprite::GetIsActive()
{
    return IsVisible() && !IsPause();
}

bool CSprite::GetIsHUD() const
{
	return m_isHUD;
}

bool CSprite::GetIsScissor() const
{
	return m_isScissor;
}

CImage* CSprite::GetImage()
{
	return m_Texture;
}

const vec2& CSprite::GetPosition() const
{
	return m_pos;
}

const vec2& CSprite::GetSize() const
{
	return m_size;
}

const vec2& CSprite::GetScale() const
{
	return m_scale;
}

const vec2& CSprite::GetAnchorPoint() const
{
	return m_anchorPoint;
}

const float& CSprite::GetAngle() const
{
	return m_angle;
}

const sColor& CSprite::GetColor() const
{
	return m_color;
}

const float& CSprite::GetOpacity() const
{
	return m_opacity;
}

bool CSprite::IsVisible(bool checkParents) 
{
    if (!checkParents)
        return m_isVisible;

    bool visible = true;
    for (CSprite* spr = this; spr; spr = dynamic_cast<CSprite*>(spr->GetParent()))
    {
        if (!spr->m_isVisible) 
        {
            visible = false;
            break;
        }
    }
	return visible;
}

const EFlip& CSprite::GetFlip() const
{
    return m_flip;
}

const EBledMode& CSprite::GetBlendMode() const
{
	return m_blendMode;
}

const CSprite::sBlendParams& CSprite::GetBlendParams() const
{
    return m_blendParams;
}

const float*  CSprite::GetVertex() const
{
    return m_vertex;
}
float*  CSprite::GetVertex()
{
    return m_vertex;
}

const float*  CSprite::GetTextCoord() const
{
    return m_textCoord;
}

vec2 CSprite::GetScreenSize()
{
    return CDirector::GetDirector().GetCanvasSize();
}

vec2 CSprite::GetPositionScreen()
{
//    return CCamera::GetCamera().ConvertWorldToScreenCoord(GetGlobalFromLocalCoord(vec2()));
    return vec2();
}

//sColor* CSprite::CreateSpriteTextRect(int& width, int& height)
//{
//    if (!m_Texture)
//    {
//        width = 0;
//        height = 0;
//        return NULL;
//    }
//    
//    ImageLib::Image *img = ImageLib::GetImage(m_Texture->GetName());
//    
//    if(img == NULL)
//    {
//        ALOG("Not implement for TexturePacker!!!!!!");
//        return NULL;
//    }
//    
//    int src_width = img->GetWidth();
//    int src_height = img->GetHeight();
//    const uint32_t* buffer = img->GetBits();
//    
//    int _width(GetNearPow2(src_width));
//    
//    int	_height(GetNearPow2(src_height));
//    
//    vec2 pos(_width * m_textCoord[0], _height * m_textCoord[3]);
//    vec2 size(( m_textCoord[4] - m_textCoord[0]) * _width, (m_textCoord[5] - m_textCoord[3]) * _height);
//      
//    sColor* retRect = new sColor[(int)(size.x * size.y)];
//    width = size.x;
//    height = size.y;
//    
//    for (int y = 0; y < size.y; y++)
//        for (int x = 0; x < size.x; x++)        
//        {
//            uint32_t point = buffer[(y + (int)pos.y) * src_width + x + (int)pos.x];
//            retRect[y * (int)size.x + x] = sColor::IntColor((point & 0xff0000) >> 16, 
//                                                            (point & 0xff00) >> 8, 
//                                                            (point & 0xff), 
//                                                            (point & 0xff000000) >> 24);
//        }
//
//    
//    delete img;
//    return retRect;
//}

CSprite* CSprite::SetImage(CImage* image)
{
	m_Texture = image;
    UpdateBlendParams();
    
    if (!image)
    {
        if (m_vertex)
            delete[] m_vertex;
        m_vertex = NULL;
        if (m_textCoord)
            delete[] m_textCoord;
        m_textCoord = NULL;
        return this;
    }
    
    if (m_color.sAlpha == 0)
        m_color.sAlpha = 1;
	
	SetTexCoord();
	m_size = vec2(m_Texture->GetWidth(), m_Texture->GetHeight());
	SetVertex();
	
	return this;
}

CSprite* CSprite::SetImage(const std::string& image)
{
	return SetImage(new CImage(image));
}

CSprite* CSprite::SetTextureCoord(const vec2& from, const vec2& to)
{
	SetTexCoord(from, to);
	return this;
}

CSprite* CSprite::TextureCoordRotate90()
{
    float temp[8];
    
    for(int i = 0; i < 8; i++)
        temp[i] = m_textCoord[i];
    
    m_textCoord[0] = temp[2];        m_textCoord[1] = temp[3];
    m_textCoord[2] = temp[6];        m_textCoord[3] = temp[7];
    m_textCoord[4] = temp[0];        m_textCoord[5] = temp[1];
    m_textCoord[6] = temp[4];        m_textCoord[7] = temp[5];
    
    m_texCoordRotated = ! m_texCoordRotated;
    
    return this;
}

CSprite* CSprite::SetPosition(const vec2& pos)
{
    if (m_pos != pos)
    {
        m_pos = pos;
        ResetMatrix();
    }
	return this;
}

CSprite* CSprite::SetPositionBy(const vec2& pos)
{
	m_pos += pos;
	ResetMatrix();
	return this;
}

CSprite* CSprite::SetSize(const vec2& size)
{
    if (m_size != size)
    {
        m_size = size;
        SetVertex();
    }
	return this;
}

CSprite* CSprite::SetSizeWithCrop(const vec2& size)
{
    if (m_textCoord && m_Texture)
    {
        vec2 kof(size.x/m_Texture->GetWidth(), size.y/m_Texture->GetHeight());
        vec2 t(m_anchorPoint.x*(1-kof.x), (1-kof.y)*(1-m_anchorPoint.y));
        SetTexCoord(t, t+kof);        
    }
    
    return SetSize(size);
}

CSprite* CSprite::SetScale(const vec2& scale)
{
    if (m_scale != scale)
    {
        m_scale = scale;
        ResetMatrix();
    }
	return this;
}
#if OS == OS_WP8
CSprite* CSprite::SetScaleVec2(const vec2& scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		ResetMatrix();
	}
	return this;
}
#endif

CSprite* CSprite::SetScale(const float& scale)
{
    if (m_scale.x != scale && m_scale.y != scale )
    {
        m_scale = vec2(scale, scale);
        ResetMatrix();
    }
	return this;
}

CSprite* CSprite::SetAnchorPoint( const int& fastPosition )
{
    switch ( fastPosition ) {
        case EFAP_CENTER:
            m_anchorPoint = vec2(0.5f, 0.5f);
            break;
            
        case EFAP_LEFT:
            m_anchorPoint = vec2(0.0f, 0.5f);
            break;
            
        case EFAP_RIGHT:
            m_anchorPoint = vec2(1.0f, 0.5f);
            break;
            
        case EFAP_BOTTOM:
            m_anchorPoint = vec2(0.5f, 0.0f);
            break;
            
        case EFAP_TOP:
            m_anchorPoint = vec2(0.5f, 1.0f);
            break;
            
        default:
            m_anchorPoint = vec2(0.0f, 0.0f);
            break;
    }
    SetVertex();
    return this;
}

CSprite* CSprite::SetAnchorPoint(const vec2& anchorPoint)
{
    if (m_anchorPoint != anchorPoint)
    {
        m_anchorPoint = anchorPoint;
        SetVertex();
    }
	
	return this;
}

CSprite* CSprite::SetAngle(const float& angle)
{
    float tempAngle = angle;
	if (tempAngle > 2*M_PI) tempAngle -= 2*M_PI;
	if (tempAngle < -2*M_PI) tempAngle += 2*M_PI;
    if (m_angle != tempAngle)
    {
        m_angle = tempAngle;
        ResetMatrix();
    }
	return this;
}

CSprite* CSprite::SetColor(const sColor& color, EBlendColor blendColor)
{
    m_blendColor = blendColor;
    return SetColor(color);
}

CSprite* CSprite::SetColor(const sColor& color)
{
	m_color = color;	
	return this;
}

CSprite* CSprite::SetOpacity(const float& op)
{
    float tempOpacity;
    tempOpacity = op>1?1:op;
    tempOpacity = op<0?0:op;
    if (m_opacity != op)
    {
        m_opacity = op;
        tempOpacity = 1;
        if (m_changeOpacityForChildren)
        {
            for (CNode* i = this->GetParent(); i; i = i->GetParent())
            {
                CSprite* spr = dynamic_cast< CSprite* > (i);
                if (spr){
                    tempOpacity = spr->GetOpacityWithParent();
                    break;
                }
            }
        }
        
        SetOpacityChild(tempOpacity);
    }
	
    return this;
}

void CSprite::SetOpacityChild(float op)
{
    m_opacityWithParent = op * m_opacity;
    
    if (!m_changeOpacityForChildren)
        return;
    
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* sp = dynamic_cast< CSprite* > (*i);
        if (sp) sp->SetOpacityChild(m_opacityWithParent);
    }
}

void CSprite::SetPremultipliedAlpha(bool isPremultipliedAlpha)
{
    m_isPremultipliedAlpha = isPremultipliedAlpha;
    UpdateBlendParams();
}

void CSprite::UpdateBlendParams()
{
    if (m_blendMode != ebmCustom)
    {
        if (m_blendMode == ebmNormal)
        {
            if (m_isPremultipliedAlpha)
                m_blendParams.Set(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            else
                m_blendParams.Set(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else if (m_blendMode == ebmAdd)
        {
            if (m_isPremultipliedAlpha)
                m_blendParams.Set(GL_ONE, GL_ONE);
            else
                m_blendParams.Set(GL_SRC_ALPHA, GL_ONE);
        }
        else if(m_blendMode == ebmMultiply)
            m_blendParams.Set(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);// old GL_DST_COLOR, GL_ZERO
        else if (m_blendMode == ebmScreen)
        {
            if (m_isPremultipliedAlpha)
                m_blendParams.Set(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
            else
                m_blendParams.Set(GL_ONE_MINUS_DST_COLOR, GL_ONE);// old
        }
    }
}

CSprite* CSprite::SetBlendMode(const EBledMode& blendMode)
{
	m_blendMode = blendMode;
    UpdateBlendParams();
    return this;
}

CSprite* CSprite::SetBlendParams(const sBlendParams& blendParams)
{
    m_blendParams = blendParams;
    m_blendMode = ebmCustom;
    return this;
}

CSprite* CSprite::SetFlip(const EFlip& flip)
{
	m_flip = flip;
	ResetMatrix();
	return this;
}

CSprite* CSprite::SetIsScissor(const bool& isScissor)
{
	m_isScissor = isScissor;
	return this;
}

void CSprite::DrawAfter(CSprite* sprite, bool pushBack)
{
	DontDrawAfter();
    m_drawAfterSprite = sprite;
    if (pushBack)
        sprite->m_drawAfterSpriteList.push_back(this);
    else
        sprite->m_drawAfterSpriteList.push_front(this);
}

void CSprite::DontDrawAfter()
{
	if (m_drawAfterSprite)
	{		
		std::list< CSprite* >::iterator i = std::find(m_drawAfterSprite->m_drawAfterSpriteList.begin(), m_drawAfterSprite->m_drawAfterSpriteList.end(), this);
		if (i != m_drawAfterSprite->m_drawAfterSpriteList.end())
		{
			m_drawAfterSprite->m_drawAfterSpriteList.erase(i);
		}
		
		m_drawAfterSprite = NULL;
	}
	
}

void CSprite::RemoveAllDrawAfter()
{
	std::list< CSprite* >::iterator i = m_drawAfterSpriteList.begin();
    for (; i != m_drawAfterSpriteList.end(); i++)
        (*i)->m_drawAfterSprite = NULL;
    m_drawAfterSpriteList.clear();
}

CSprite* CSprite::PlaceObjectsInRow(float spaceY)
{
    vec2 currPos = vec2(-m_size.width*0.5f, m_size.height*0.5f);
    for (std::list< CNode* >::iterator i = GetChildren().begin(); i != GetChildren().end(); i++)
    {
        CSprite* sp = dynamic_cast< CSprite* > (*i);
        if (sp && sp->GetSize() != vec2(0, 0) && !sp->IsDelete())
        {            
            if (currPos.x + sp->GetSize().x >= m_size.width*0.5f)
            {
                currPos.x = -m_size.width*0.5f;
                currPos.y -= (spaceY==0.0f) ? sp->GetSize().y : spaceY;
            }
            vec2 spSize = vec2(sp->GetSize().x * sp->GetAnchorPoint().x, sp->GetSize().y * (1 - sp->GetAnchorPoint().y));
            sp->SetPosition(currPos+vec2(spSize.x, (spaceY==0.0f) ? -spSize.y : -spaceY*0.5f));
            currPos.x += sp->GetSize().x;
        }
    }
    return this;
}

CSprite* CSprite::PlaceObjectsAs(fSRectHW SizeRect,vec2 space)
{
    float X=0;
    float Y=0;
    
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* sp = dynamic_cast< CSprite* > (*i);
        if (sp && sp->GetSize() != vec2(0, 0) && !sp->IsDelete())
        {//h(1-a)
            float ASizeX=sp->m_size.x * (1-sp->m_anchorPoint.x);
            
            vec2 CalcPosition=vec2(SizeRect.x+X*space.x,SizeRect.y+Y*space.y);
            if ((CalcPosition.x+ASizeX)>SizeRect.w)
            {
                X=0;
                Y-=1;
                CalcPosition=vec2(SizeRect.x+X*space.x,SizeRect.y+Y*space.y);
            }
            sp->SetPosition(CalcPosition);
            X+=1;
        }
    }
    return this;
}

CSprite* CSprite::PlaceObjectsAs(EPlaceStyle style, float space,float alignPos)
{
	vec2 pos(0, 0);

    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* sp = dynamic_cast< CSprite* > (*i);
        if (sp && sp->GetSize() != vec2(0, 0) && !sp->IsDelete())
        {
            vec2 alignPoint = (style == epsColomn) ? vec2(alignPos, 1) : vec2(0, alignPos);
            
            sp->SetPosition(pos + vec2(sp->m_size.x * (sp->m_anchorPoint.x - alignPoint.x), sp->m_size.y * (sp->m_anchorPoint.y - alignPoint.y)));
            if (style == epsLine )
                pos.x += sp->m_size.x + space;
            else if (style == epsColomn)
                pos.y -= sp->m_size.y + space;
        }
    }
	

	vec2 len = (style == epsColomn) ? vec2(0, (pos.y - space) * (1 - m_anchorPoint.y)) : vec2((pos.x - space) * m_anchorPoint.x, 0);
	
    i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* sp = dynamic_cast< CSprite* > (*i);
        if (sp && sp->GetSize() != vec2(0, 0))
        {
            sp->SetPosition(sp->GetPosition() - len);
        }
    }
	
	
	return this;
}

vec2 CSprite::GetGlobalFromLocalCoord( const vec2 &_pos )
{
    if ( !m_matrixSave )
    {
        if (GetParent() || GetChildren().size())
        {
            std::vector<CSprite*> stackOfParents;
            if (!IsOnScreen(stackOfParents))
                RecalculateMatrixAndUpdateParents(stackOfParents);
            
            if (!m_matrixSave)
                return _pos;
        }
        else
        {
            return _pos;
        }
    }

    mat4 _matrix = mat4( m_matrixSave[0], m_matrixSave[4], m_matrixSave[8], m_matrixSave[12],
                         m_matrixSave[1], m_matrixSave[5], m_matrixSave[9], m_matrixSave[13],
                         m_matrixSave[2], m_matrixSave[6], m_matrixSave[10], m_matrixSave[14],
                         m_matrixSave[3], m_matrixSave[7], m_matrixSave[11], m_matrixSave[15] ); //Транспонирование матрицы вида
   
    //Вычисление результирующей точки
    vec3 res_Pos = _matrix * vec3( _pos.x, _pos.y, 0.0f );
    return vec2( res_Pos.x, res_Pos.y );
}

vec2 CSprite::GetLocalFromGlobalCoord( const vec2 &_pos )
{
    if ( !m_matrixSave )
    {
        if (GetParent() || GetChildren().size())
        {
            std::vector<CSprite*> stackOfParents;
            if (!IsOnScreen(stackOfParents))
                RecalculateMatrixAndUpdateParents(stackOfParents);
            
            if (!m_matrixSave)
                return _pos;
        }
        else
        {
            return _pos;
        }
    }
    
    mat4 _matrix = mat4( m_matrixSave[0], m_matrixSave[4], m_matrixSave[8], m_matrixSave[12],
                        m_matrixSave[1], m_matrixSave[5], m_matrixSave[9], m_matrixSave[13],
                        m_matrixSave[2], m_matrixSave[6], m_matrixSave[10], m_matrixSave[14],
                        m_matrixSave[3], m_matrixSave[7], m_matrixSave[11], m_matrixSave[15] ); //Транспонирование матрицы вида
    mat4 _matRes = _matrix.inverse(); // Получаем обратную матрицу
    vec3 res_Pos = _matRes * vec3( _pos.x, _pos.y, 0.0f ); //Вычисляем 
    return vec2( res_Pos.x, res_Pos.y );
}

bool CSprite::IsPointInSprite( const vec2& _point )
{
    if (m_matrixSave && m_matrixSave[0] == 0.0f && m_matrixSave[5] == 0.0f)
    {
        ALOG("Sprite %s scale is 0,0! IsPointInSprite = false", GetName().c_str());
        return false;
    }
	vec2 _localPoint = GetLocalFromGlobalCoord( _point );
	
	if (m_size == vec2(0, 0)) 
	{   
		if (_localPoint == vec2(0, 0)) return true;
		else return	false;
	}		
    
    if ( _localPoint.x < m_vertex[0] || _localPoint.x > m_vertex[4] || 
         _localPoint.y < m_vertex[1] || _localPoint.y > m_vertex[3] )
    {
        return false;
    }
    return true;
}

//#include "CIntersection.h"

bool CSprite::IntersectWithCamera()
{
    float _objRectU_vertex[12];
    for(int i = 0;i < 12;i++)
        _objRectU_vertex[i] = 0.0f;
    vec2 _bl =  GetGlobalFromLocalCoord( vec2( this -> GetVertex()[0] ,
                                               this -> GetVertex()[1] ) );
    vec2 _tl =  GetGlobalFromLocalCoord( vec2( this -> GetVertex()[2] ,
                                               this -> GetVertex()[3]  ) );
    vec2 _tr =  GetGlobalFromLocalCoord( vec2( this -> GetVertex()[6] ,
                                               this -> GetVertex()[7]  ) );
    vec2 _br =  GetGlobalFromLocalCoord( vec2( this -> GetVertex()[4] ,
                                               this -> GetVertex()[5]  ) );
    _objRectU_vertex[0] = _bl.x; _objRectU_vertex[1] = _bl.y; //2
    _objRectU_vertex[3] = _tl.x; _objRectU_vertex[4] = _tl.y; //5
    _objRectU_vertex[6] = _tr.x; _objRectU_vertex[7] = _tr.y; //8
    _objRectU_vertex[9] = _br.x; _objRectU_vertex[10] = _br.y;//11

//    float* _objRect_vertex = CCamera::GetCamera().GetIntersectionPts();
//
//    for ( int i = 0; i < 12; i += 3 )
//        if ( Check_Point_InRect( _objRectU_vertex[i], _objRectU_vertex[i+1], _objRect_vertex ) )
//            return true;
//    for ( int i = 0; i < 12; i += 3 )
//        if ( Check_Point_InRect( _objRect_vertex[i], _objRect_vertex[i+1], _objRectU_vertex ) )
//            return true;
//    for ( int k = 0, i = 0; i < 12; i += 3 )
//    {
//        k = i + 3;
//        if ( k > 9 ) k = 0;
//        for ( int l = 0, j = 0; j < 12; j += 3 )
//        {
//            l = j + 3;
//            if ( l > 9 ) l = 0;
//            if ( edgeIntersection( vec2( _objRectU_vertex[j], _objRectU_vertex[j+1] ),
//                                  vec2( _objRectU_vertex[l], _objRectU_vertex[l+1] ),
//                                  vec2( _objRect_vertex[i], _objRect_vertex[i+1] ),
//                                  vec2( _objRect_vertex[k], _objRect_vertex[k+1] )))
//                return true;
//        }
//    }

    return false;
}

//CSprite* CSprite::Correct(std::string name)
//{
//    AddChangedSpritePosition(this, name);
//    return this;
//}
//
//CSprite* CSprite::CorrectSize(std::string name)
//{
//    AddChangedSpriteSize(this, name);
//    return this;
//}
//
//CSprite* CSprite::CorrectScale(std::string name)
//{
//    AddChangedSpriteScale(this, name);
//    return this;
//}
//
//CSprite* CSprite::CorrectOpacity(std::string name)
//{
//    AddChangedSpriteOpacity(this, name);
//    return this;
//}
//
//CSprite* CSprite::CorrectHSV(std::string name)
//{
//    AddChangedVariable(&m_color[0], name + "_H");
//    AddChangedVariable(&m_color[1], name + "_S");
//    AddChangedVariable(&m_color[2], name + "_V");
//    return this;
//}

bool CSprite::IsOnScreen(std::vector<CSprite*>& stackOfParents)
{
    const CNode* subroot = CDirector::GetDirector().GetRooot();
    
    bool isOnScreen = (this == subroot) ? true : false;
    for (CNode* parent = GetParent(); parent; parent = parent->GetParent())
    {
        if (parent == subroot)
            isOnScreen = true;
        
        if (CSprite* spr = dynamic_cast< CSprite* > (parent))
            stackOfParents.push_back(spr);
    }

    return isOnScreen;
}


void CSprite::RecalculateMatrixAndUpdateParents(std::vector<CSprite*>& stackOfParents)
{
    Render::PushMatrix();
    Render::LoadIdentity();
    
    for (std::vector<CSprite*>::reverse_iterator it = stackOfParents.rbegin(); it != stackOfParents.rend(); ++it) {
        (*it)->SetMatrix();
    }
         
    ResetMatrix();
    SetMatrixRecur();

    Render::PopMatrix();
}


CNode* CSprite::AddChild(CNode* node)
{
    if (!CNode::AddChild(node))
        return NULL;

    if (CSprite* s = dynamic_cast<CSprite*> (node))
	{
        std::vector<CSprite*> stackOfParents;
        stackOfParents.push_back(this);

        if (IsOnScreen(stackOfParents))
        {
            s->RecalculateMatrixAndUpdateParents(stackOfParents);
            s->SetOpacityChild(m_changeOpacityForChildren ? m_opacityWithParent : 1.f);
        }
	}

    return this;
}

CNode* CSprite::AddChild(CNode* node, int z)
{
	return CNode::AddChild(node, z);
}

CSprite* CSprite::GetCopy()
{
	return new CSprite(*this);
}

void CSprite::SaveScissor()
{
    if (m_saveScissor == NULL)
        m_saveScissor = new int[4];

    m_isSaveScissor = true;
    glGetIntegerv(GL_SCISSOR_BOX, m_saveScissor);
}

void CSprite::EnableScissor(bool isRotated)
{
    const float* points = GetVertex();
    if (points == NULL)
        return;
    
//    if (isRotated) //StencilBuffer
//	{
//		bool isScissor = glIsEnabled(GL_SCISSOR_TEST);
//		if (isScissor)
//			glDisable(GL_SCISSOR_TEST);
//
//        Render::ClearStencilBuffer();
//        Render::EnableStencilTest();
//        
//        Render::ColorMask(false,false,false,false);
//        Render::StencilFunc(GL_ALWAYS, 1, 1);
//        Render::StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//        
//        //draw mask
//        if (CDirector::GetDirector().IsShadersOn())
//            CShaderManager::getInstance()->SetActiveShader(CShaderManager::eST_Default);
//        else
//            glDisable(GL_TEXTURE_2D);
//        Render::DisableClientState(GL_TEXTURE_COORD_ARRAY);
//        Render::Color4f(1, 1, 1, 1);
//        Render::EnableClientState(GL_VERTEX_ARRAY);
//        Render::VertexPointer(2, GL_FLOAT, 0, points);
//        Render::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//        
//        Render::ColorMask(true,true,true,true);
//        
//        Render::StencilFunc(GL_NOTEQUAL,0 , 1);
//        Render::StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
//
//		if (isScissor)
//			glEnable(GL_SCISSOR_TEST);
//    }
//    else //Scissor
//    {
//        vec2 p0 = vec2(points[0], points[1]);
//        vec2 p1 = vec2(points[6], points[7]);
//        
//        if (GetIsHUDThisOrParent())
//        {
//            p0 = CCamera::GetCamera().GetHudCoordByCameraCoord( GetGlobalFromLocalCoord(p0) );
//            p1 = CCamera::GetCamera().GetHudCoordByCameraCoord( GetGlobalFromLocalCoord(p1) );
//        }
//        else
//        {
//            p0 = CCamera::GetCamera().GetCameraCoord( GetGlobalFromLocalCoord(p0) );
//            p1 = CCamera::GetCamera().GetCameraCoord( GetGlobalFromLocalCoord(p1) );
//        }
//        
//        vec2 minPoint(MIN(p0.x, p1.x), MIN(p0.y, p1.y));
//        p1.set(MAX(p0.x, p1.x), MAX(p0.y, p1.y));
//        p0 = minPoint;
//        
//        p1 -= p0;
//        
//        p0.x *=  CCamera::GetCamera().GetViewPortSize().x /  CCamera::GetCamera().GetOrthoSize().x;
//        p0.y *=  CCamera::GetCamera().GetViewPortSize().y /  CCamera::GetCamera().GetOrthoSize().y;
//        
//        p1.x *=  CCamera::GetCamera().GetViewPortSize().x /  CCamera::GetCamera().GetOrthoSize().x;
//        p1.y *=  CCamera::GetCamera().GetViewPortSize().y /  CCamera::GetCamera().GetOrthoSize().y;
//        
//        vec2 rect_lb = p0; //left bottom
//        vec2 rect_rt = p0 + p1; //right top
//        vec2 screen_lb = vec2();
//        vec2 screen_rt = CCamera::GetCamera().GetViewPortSize();
//        vec2 correct_rect_lb;
//        vec2 correct_rect_rt;
//        
//        if( rect_rt.y < screen_lb.y || rect_lb.y > screen_rt.y || rect_lb.x > screen_rt.x || rect_rt.x < screen_lb.x )
//        {
//            //do not intersect
//            if (glIsEnabled(GL_SCISSOR_TEST))
//                SaveScissor();
//            return;
//        }
//        else
//        {
//            correct_rect_lb.y=max(rect_lb.y,screen_lb.y);
//            correct_rect_lb.x=max(rect_lb.x,screen_lb.x);
//            correct_rect_rt.y=min(rect_rt.y,screen_rt.y);
//            correct_rect_rt.x=min(rect_rt.x,screen_rt.x);
//        }
//        
//        p0 = correct_rect_lb;
//        p1 = correct_rect_rt - correct_rect_lb;
//        
//#if OS_WP8_0
//        if (CDirector::GetDirector().GetOrientation() == ITO_LandscapeLeft)
//        {
//            p0 = vec2(p0.y, CCamera::GetCamera().GetViewPortSize().x - p0.x - p1.x);
//            p1 = vec2(p1.y, p1.x);
//        }
//		else if (CDirector::GetDirector().GetOrientation() == ITO_LandscapeRight)
//        {
//            p0 = vec2(CCamera::GetCamera().GetViewPortSize().y - p0.y - p1.y, p0.x);
//            p1 = vec2(p1.y, p1.x);
//        }
//#endif
//        
//        if (glIsEnabled(GL_SCISSOR_TEST))
//        {
//            SaveScissor();
//            
//            if(( m_saveScissor[0] + m_saveScissor[2] >= p0.x && p0.x + p1.x >= m_saveScissor[0] ) &&
//               ( m_saveScissor[1] + m_saveScissor[3] >= p0.y && p0.y + p1.y >= m_saveScissor[1] )    )
//            {
//                vec2 c(p0+p1);
//                
//                p0.x=MAX(m_saveScissor[0], p0.x);
//                p0.y=MAX(m_saveScissor[1], p0.y);
//                
//                p1.x = MIN(m_saveScissor[0]+m_saveScissor[2], c.x) - p0.x;
//                p1.y = MIN(m_saveScissor[1]+m_saveScissor[3], c.y) - p0.y;
//            }
//            else
//            {
//                p0.x = 0, p0.y = 0, p1.x = 0, p1.y = 0;
//            };
//        }
//        else
//            glEnable(GL_SCISSOR_TEST);
//        
//        glScissor(p0.x, p0.y, p1.x, p1.y);
//    }
}
void	CSprite::DisableScissor(bool isRotated)
{
    if (isRotated)
    {
//        Render::DisableStencilTest();
    }
    else
    {
        if (m_isSaveScissor && m_saveScissor)
        {
            glScissor(m_saveScissor[0], m_saveScissor[1], m_saveScissor[2], m_saveScissor[3]);
            m_isSaveScissor = false;
        }
        else
            glDisable(GL_SCISSOR_TEST);
    }
}

float CSprite::GetOpacityWithParent()
{
    return m_opacityWithParent;
}

void CSprite::DisableRenderWithFBO()
{
//    if ( m_fbo )
//        CDirector::GetFBOManager().RemoveFBO(m_fboID, this);
    if (m_isNotCalculationToFBO)
        SetNotCalculationToFBO(false);
    m_fbo = NULL;
    m_fboID = -1;
}

void CSprite::EnableRenderWithFBO(eFBORenderMode mode)
{
//    if(!CDirector::GetDirector().IsEnabledFBORender())
//        return;
//    
//    if(m_fbo != NULL)
//        return;
//    m_fboID = CDirector::GetFBOManager().GenerateUniqueID();
//    m_fbo = CDirector::GetFBOManager().AddFBO(m_fboID, this, GetSize(), mode);
}

void CSprite::EnableRenderWithFBO(int ID, eFBORenderMode mode)
{
//    if(!CDirector::GetDirector().IsEnabledFBORender())
//        return;
//    if(m_fbo != NULL)
//        return;
//    
//    m_fboID = ID;
//    m_fbo = CDirector::GetFBOManager().AddFBO(m_fboID, this, GetSize(), mode);
}

void CSprite::EnableStencilForFBO()
{
    if(m_fbo == NULL)
        return;
//    m_fbo->EnableStencilBuffer();
}

void CSprite::SetFBORenderMode(int mode)
{
    if(m_fbo == NULL)
        return;
//    m_fbo->SetRenderMode((CFrameBufferObject::eRenderMode)mode);
}

void CSprite::SetNotCalculationToFBO(bool val)
{
    m_isNotCalculationToFBO = val;
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* s = dynamic_cast< CSprite* > (*i);
        if (s) s->SetNotCalculationToFBO(val);
    }
}

void CSprite::SetOnlyGenerateFBO(bool val)
{
    m_isOnlyGenerateFBO = val;
    SetVisible(false);
}

void CSprite::PrepareRenderToFBORecur()
{
    PrepareRenderToFBO();
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* s = dynamic_cast< CSprite* > (*i);
        if (s) s->PrepareRenderToFBORecur();
    }
}

void CSprite::PrepareRenderToFBO()
{
    
}


void CSprite::SetRenderToFBO(bool val)
{
    m_isRenderToFBO = val;
    std::list< CNode* >::iterator i = GetChildren().begin();
    for (; i != GetChildren().end(); i++)
    {
        CSprite* s = dynamic_cast< CSprite* > (*i);
        if (s) s->SetRenderToFBO(val);
    }
}

void CSprite::SetHSV(const float& h, const float& s, const float& v)
{
    m_color[0] = h;
    m_color[1] = s;
    m_color[2] = v;
    
    m_blendColor = EBC_HSV;
}


void CSprite::EnableRenderProfiling(bool value, std::string name)
{
#ifdef ENABLE_PROFILING
    m_enableRenderProfiling = value;
    if(name == "")
        m_profilingName = GetName();
    else
        m_profilingName = name;
#endif
}

//sImageHelper CSprite::GetFboImageInfo()
//{
//    sImageHelper result;
//    if(m_fbo)
//    {
//        SetVisible(true);
//        m_fbo->UpdateTexture(this);
//        SetVisible(false);
//        result = m_fbo->GetImageInfo();
//    }
//    return result;
//}


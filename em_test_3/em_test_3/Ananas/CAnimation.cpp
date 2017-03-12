/*
 *  CAnimation.cpp
 *  Ananas 
 *
 *  Created by Anton Zagovalko on 27.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 * 
 */

#include "CAnimation.h"
#include "CDirector.h"
#include "tinyxml.h"
#include "Trace.hpp"
#include "Utils.hpp"
#include "FileLoader.hpp"
#include <algorithm>
#include <math.h>

struct _SCadr
{
    int _name;
    std::string _path;
    vec2        _pos;
    _SCadr( int name, std::string& path, vec2 pos )
    {
        _name = name;
        _path = path;
        _pos = pos;
    }
    static bool MyDataSortPredicate(const _SCadr& d1, const _SCadr& d2)
    {
        return d1._name < d2._name;
    }
};


CAnimation::CAnimation()
:m_currentFrame(	0),
m_time(				0),
m_frameTime(		1.0f/24),
m_isCycle(			false),
m_isPlay(			false)
{
    SetColor(sColor(1,1,1,1));
}

CAnimation::CAnimation(CAnimation& anim)
:CSprite(		anim),
m_currentFrame(	0),
m_time(			0),
m_frameTime(	anim.m_frameTime),
m_isCycle(		anim.m_isCycle),
m_isPlay(		anim.m_isPlay)
{
	for (int i = 0; i < anim.m_frames.size(); i++)
	{
		m_frames.push_back(anim.m_frames[i]->GetCopy());
	}
}

CAnimation::~CAnimation()
{
	ClearFrames();
}

void CAnimation::ClearFrames()
{
    for (int i = 0; i < m_frames.size(); i++) 
	{
		delete m_frames[i];
        m_frames[i] = NULL;
	}
    m_frames.clear();
    m_frameEvents.clear();
    m_currentFrame = 0;
}

CAnimation* CAnimation::AddFrame(std::string path)
{
	m_frames.push_back(new SAnimationFrame(path));
    SetFrame();
	return this;
}

CAnimation* CAnimation::AddFrame(std::string pathMask, int from, int to)
{
    if(from < to)
    {
        for (int i = from; i <= to; i++)
        {
            AddFrame(G_StringFormat(pathMask, i));
        }
    }
    else 
    {
        for (int i = from; i >= to; i--)
        {
            AddFrame(G_StringFormat(pathMask, i));
        }
    }
    return this;
}

CAnimation* CAnimation::AddFrameXML(std::string pathToXMLFolder)
{
    TiXmlDocument _xml;
    std::string fileContent = gReadFile(pathToXMLFolder + std::string("imageOffset.xml"), "rb");
    _xml.Parse( fileContent.c_str() );

    std::vector<_SCadr> _Cadrs;
    
	TiXmlElement* first = _xml.FirstChildElement("imageSize");
	if ( first )
	{
        double speed(0);
        first ->Attribute("speed", &speed);
        if (speed) SetAnimationSpeed(speed);
        
		TiXmlElement* image = first -> FirstChildElement( "image" );
		while ( image )
		{
			const char* name = image -> Attribute( "name" );
			std::string str = std::string( name );
            size_t found = str.find( "." );
            int num = 0;
            if ( found != std::string::npos )
            {
                int dec_count = 0;
                while(found)
                {
                    std::string substr = str.substr(found-1, found);
                    char tmp[2];
                    tmp[0] = substr[0];
                    tmp[1] = '\0';
                    if(isdigit(tmp[0]))
                        num += atoi(tmp)*(pow(10.0,dec_count));
                    else break;
                    dec_count++;
                    found--;
                }
            }
			         
            std::string bufFile = G_StringFormat("%s%s", pathToXMLFolder.c_str(), name);
            
			double SX;
			double SY;
			image -> Attribute( "offsetX", &SX );
			image -> Attribute( "offsetY", &SY );
            
            _Cadrs.push_back( _SCadr( num, bufFile, vec2( -SX, SY ) ) );
			image = image -> NextSiblingElement( "image" );
		}
    }
    std::sort(  _Cadrs.begin(), _Cadrs.end(), _SCadr::MyDataSortPredicate );
        
    for ( int i = 0; i < _Cadrs.size(); i ++ )
        AddFrame( _Cadrs[i]._path, _Cadrs[i]._pos );
    
    return this;
}

CAnimation* CAnimation::AddFrameXMLPercent(std::string pathToXMLFolder, float framePercent)
{
    TiXmlDocument _xml;
    std::string fileContent = gReadFile(pathToXMLFolder + std::string("imageOffset.xml"), "rb");
    _xml.Parse( fileContent.c_str() );
    
    std::vector<_SCadr> _Cadrs;
    
	TiXmlElement* first = _xml.FirstChildElement("imageSize");
	if ( first )
	{
        double speed(0);
        first ->Attribute("speed", &speed);
        if (speed) SetAnimationSpeed(speed);
        
		TiXmlElement* image = first -> FirstChildElement( "image" );
		while ( image )
		{
			const char* name = image -> Attribute( "name" );
			std::string str = std::string( name );
            size_t found = str.find( "." );
            int num = 0;
            if ( found != std::string::npos )
            {
                int dec_count = 0;
                while(found)
                {
                    std::string substr = str.substr(found-1, found);
                    char tmp[2];
                    tmp[0] = substr[0];
                    tmp[1] = '\0';
                    if(isdigit(tmp[0]))
                        num += atoi(tmp)*(pow(10.0,dec_count));
                    else break;
                    dec_count++;
                    found--;
                }
            }
			
            std::string bufFile = G_StringFormat("%s%s", pathToXMLFolder.c_str(), name);
            
			double SX;
			double SY;
			image -> Attribute( "offsetX", &SX );
			image -> Attribute( "offsetY", &SY );
            
            _Cadrs.push_back( _SCadr( num, bufFile, vec2( -SX, SY ) ) );
			image = image -> NextSiblingElement( "image" );
		}
    }
    std::sort(  _Cadrs.begin(), _Cadrs.end(), _SCadr::MyDataSortPredicate );
    
    int del = (framePercent < 1.0f) ? (int)(1.0f/framePercent) : 0;
    if (_Cadrs.size() <= del*2)
        del = 0;
    
    for ( int i = 0; i < _Cadrs.size(); i ++ )
    {
        if (del)
        {
            if (i%del==0)
                AddFrame( _Cadrs[i]._path, _Cadrs[i]._pos );
        }
        else
            AddFrame( _Cadrs[i]._path, _Cadrs[i]._pos );
    }
    
    return this;
}

CAnimation* CAnimation::AddFrame(std::string path, vec2 shift)
{
	m_frames.push_back(new SAnimationFrameShift(path, shift));
    SetFrame();
	return this;
}


CAnimation* CAnimation::AddFrame(CImage* texture, vec2 from, vec2 to, vec2 shift)
{
    m_frames.push_back(new SAnimationFrameTexturePart(texture, from, to, shift));
    SetFrame();
	return this;
}

CAnimation*	CAnimation::AddFrame(CImage* texture, sFrameInfo info, sMetaData metadata)
{
    m_frames.push_back(new SAnimationFrameTexturePacker(texture, info, metadata));
    SetFrame();
	return this;
}

CAnimation* CAnimation::SetAnimationSpeed(float fps)
{
	m_frameTime = 1 / fps;
	return this;
}

CAnimation* CAnimation::SetAnimationPlayTime(float time)
{
	m_frameTime = time / m_frames.size();
	return this;
}

CAnimation* CAnimation::SetFrameStartEvent(CEvent ev, int frame)
{    
	m_frameEvents.insert(std::pair<int, CEvent>(frame, ev));
	return this;
}

int CAnimation::GetFrameNum()
{
	return m_frames.size();
}

CAnimation* CAnimation::RunAnimation(bool isCycle, int frame)
{
	m_isPlay = true;
	m_isCycle = isCycle;
	if (frame >= m_frames.size() || frame < 0)
		m_currentFrame = 0;
	else 
		m_currentFrame = frame;
	SetFrame();
	return this;
}

CAnimation* CAnimation::StopAnimation()
{
	m_isPlay = false;
	m_time = 0;
	return this;
}

CAnimation* CAnimation::SetFrame(int frame)
{
    m_currentFrame=frame;
    SetFrame();
	return this;
}

void CAnimation::Update(float dt)
{
	if (m_isPlay)
    {
        if (m_time == 0) CallEvent();
        
		m_time += dt;
		while (m_time >= m_frameTime) 
		{
			m_time -= m_frameTime;
			m_currentFrame++;
			
            // конец анимации
            if (m_currentFrame >= m_frames.size())
            {
                bool bExit = false;
                if (!m_isCycle)
                {
                    StopAnimation();
                    bExit = true;
                }
                CallEvent();
                
                if(bExit) return;
                
                if (m_currentFrame >= m_frames.size()) m_currentFrame = 0;
            }

            SetFrame();

            CallEvent();
        }
	}
}

CAnimation* CAnimation::GetCopy()
{
	return new CAnimation(*this);
}

void CAnimation::CallEvent()
{
    // вызов событий
    std::map< int, CEvent >::iterator i = m_frameEvents.find(m_currentFrame);
    if (i != m_frameEvents.end())
    {
        SFrameShowData frameData(m_currentFrame, this);
        i->second.RunEvent(&frameData);
    }
}

void CAnimation::SetFrame()
{    
    if(!m_frames.empty())
    {
        m_frames[m_currentFrame]->SetFrameTo( this );
    }
    else
    {
        ALOG("Frames not loaded");
    }
}

CImage* CAnimation::GetFrameImageByNum(int frameNum)
{
    return m_frames[frameNum-1]->GetImage();
}

vec2 CAnimation::GetFrameShiftByNum(int frameNum)
{
    SAnimationFrameShift* frame = dynamic_cast <SAnimationFrameShift*> (m_frames[frameNum-1]);
    if(frame)
        return frame->GetShift();
    return vec2();
}

void CAnimation::Reverse()
{
    reverse(m_frames.begin(), m_frames.end());
}

///////////////////////////// Установка кадров //////////////////////////

CAnimation::SAnimationFrame::SAnimationFrame(std::string imagePath)
{
    SetImage(imagePath);
}

CAnimation::SAnimationFrame::SAnimationFrame(CImage* img)
: m_image(img)
{
}

CAnimation::SAnimationFrame::~SAnimationFrame()
{
    if (m_image)
    {
        delete m_image;
        m_image = NULL;
    }
}

void CAnimation::SAnimationFrame::SetImage(const std::string& imagePath)
{
    m_image = new CImage(imagePath);
}

bool CAnimation::SAnimationFrame::SetFrameTo(CSprite* anim)
{
    if (m_image && m_image->GetWidth() != 0 && m_image->GetHeight() != 0)
    {
        anim->SetImage(m_image);
        return true;
    }
    return false;
}

bool CAnimation::SAnimationFrameShift::SetFrameTo(CSprite* anim)
{
	if (!SAnimationFrame::SetFrameTo(anim)) 
        return false;
	anim->SetAnchorPoint(vec2(-m_shift.x / anim->GetSize().x + 0.5, -m_shift.y / anim->GetSize().y + 0.5));
    return true;
}

bool CAnimation::SAnimationFrameTexturePart::SetFrameTo(CSprite* anim)
{
    if (!SAnimationFrame::SetFrameTo(anim))
        return false;
	anim->SetTextureCoord(m_from, m_to);
    anim->SetSize(vec2(anim->GetSize().x*(m_to.x-m_from.x),anim->GetSize().y*(m_to.y-m_from.y)));
    anim->SetAnchorPoint(vec2(-m_shift.x / anim->GetSize().x + 0.5, -m_shift.y / anim->GetSize().y + 0.5));
    return true;
}

bool CAnimation::SAnimationFrameTexturePacker::SetFrameTo(CSprite*anim)
{
    if (!SAnimationFrame::SetFrameTo(anim))
        return false;
    CTexturePackerLoader::ApplySpriteParameters(anim, mFrameInfo, mMetaData);
    return true;
}

void CAnimation::ShowAnimation(void*)
{
    SetVisible(true);
}

void CAnimation::HideAnimation(void*)
{
    SetVisible(false);
}
void CAnimation::Active()
{
    for(int i=0;i<m_frames.size();i++)
    {
        if (m_frames[i]->GetImage()) 
            ;// CDirector::GetImageManager().Activ(m_frames[i]->GetImage());
            //m_frames[i]->GetImage()->Activ();
    }
}

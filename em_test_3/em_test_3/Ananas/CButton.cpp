/*
 *  CButton.cpp
 *  Casino
 *
 *  Created by Anton Zagovalko on 30.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CButton.h"
#include "Actions/CActionSequence.h"
#include "Actions/CActionRunEvent.h"
#include "Actions/CActionWaiting.h"
#include "Actions/CActionSimultaneously.h"
#include "Actions/CActionModifiers.h"

CButton::CButton()
: m_normal(		NULL),
m_pressed(		NULL),
m_disable(		NULL),
m_pressDetecter(new CSprite),
m_shift(		0, 0),
m_pressedShift( 0, 0),
m_scaleshift(	1, 1),
m_angle(        0),
m_selectEvent(	CEvent()),
m_unSelectEvent(CEvent()),
m_pressEvent(	CEvent()),
m_isSelect(		false),
m_OnOverButton( false),
m_swichTime(	0),
m_isEnable(		true),
m_isNormalScaleAnim( false),
m_normalScaleAnim(NULL),
m_procActive(	NULL),
m_procNormal(	NULL),
m_eventData(     NULL),
m_alphaTouch(   NULL),
m_tochID(       0),
m_normalStartPos(vec2(0,0)),
m_pressedStartPos(vec2(0,0))
{	
	AddChild(m_pressDetecter);
	SetColor(sColor(0,0,0,0));
}

CButton::CButton(CSprite* normal, vec2 shift)
: m_normal(		normal),
m_pressed(		NULL),
m_disable(		NULL),
m_pressDetecter(new CSprite),
m_shift(		shift),
m_pressedShift( 0, 0),
m_angle(        0),
m_scaleshift(	1, 1),
m_selectEvent(	CEvent()),
m_unSelectEvent(CEvent()),
m_pressEvent(	CEvent()),
m_isSelect(		false),
m_OnOverButton( false),
m_swichTime(	0),
m_isEnable(		true),
m_isNormalScaleAnim( false),
m_normalScaleAnim(NULL),
m_procActive(	NULL),
m_procNormal(	NULL),
m_eventData(     NULL),
m_alphaTouch(   NULL),
m_tochID(       0),
m_normalStartPos(vec2(0,0)),
m_pressedStartPos(vec2(0,0))
{
	AddChild(m_pressDetecter);
	SetNormal(normal);
}

CButton::CButton(CSprite* normal, CSprite* pressed, CSprite* disabled)
: m_normal(		normal),
m_pressed(		pressed),
m_disable(		disabled),
m_pressDetecter(new CSprite),
m_shift(		0, 0),
m_pressedShift( 0, 0),
m_angle(        0),
m_scaleshift(	1, 1),
m_selectEvent(	CEvent()),
m_unSelectEvent(CEvent()),
m_pressEvent(	CEvent()),
m_isSelect(		false),
m_OnOverButton( false),
m_swichTime(	0),
m_isEnable(		true),
m_isNormalScaleAnim( false),
m_normalScaleAnim(NULL),
m_procActive(	NULL),
m_procNormal(	NULL),
m_eventData(     NULL),
m_alphaTouch(   NULL),
m_tochID(       0),
m_normalStartPos(vec2(0,0)),
m_pressedStartPos(vec2(0,0))
{       
	AddChild(m_pressDetecter);
	SetNormal(normal, false);
    if (normal == pressed)
        m_pressed = NULL;
    else
        SetPressed(pressed, false);

    if(m_disable) SetDisabled(disabled, false);
}

CButton::CButton(const CButton& but)
:CSprite(			but),
m_normal(           NULL),
m_pressed(          NULL),
m_disable(          NULL),
m_shift(			but.m_shift),
m_pressedShift(     but.m_pressedShift),
m_scaleshift(		but.m_scaleshift),
m_angle(            but.m_angle),
m_isSelect(			false),
m_OnOverButton(     false),
m_isNormalScaleAnim( false),
m_normalScaleAnim(NULL),
m_procActive(		NULL),
m_procNormal(		NULL),
m_eventData(         but.m_eventData),
m_alphaTouch(       NULL),
m_tochID(           0),
m_normalStartPos(vec2(0,0)),
m_pressedStartPos(vec2(0,0))
{	
	if (but.m_normal)
	{
        int z = but.m_normal->GetZOrder();
		m_normal = (CSprite*)GetChildByZ(z);
        m_normalStartPos = m_normal->GetPosition();
	}
	if (but.m_pressed)
	{
        int z = but.m_pressed->GetZOrder();
		m_pressed = (CSprite*)GetChildByZ(z);
        m_pressedStartPos = m_pressed->GetPosition();
		m_pressed->SetVisible(false);
		m_pressed->SetPause(true);
	} 
	if (but.m_disable)
	{
		int z = but.m_disable->GetZOrder();
		m_disable = (CSprite*)GetChildByZ(z);
		m_disable->SetPause(true);
		m_disable->SetVisible(false);
	} 
    
    int z = but.m_pressDetecter->GetZOrder();
    m_pressDetecter = (CSprite*)GetChildByZ(z);
	
	SetSmoothSwich(but.m_swichTime);
	SetEnable(but.m_isEnable);
    
    if (but.m_alphaTouch)
        SetIsAlphaIgnore(true);
}

CButton::~CButton()
{
    if (m_alphaTouch)
        delete[] m_alphaTouch;
}

CButton* CButton::SetNormal(CSprite* normal,bool delActions)
{
    if (delActions)
        RemoveButtonActions();
    
	if (m_normal) RemoveChild(m_normal, true);
	m_normal = normal;
    if (m_normal)
    {
        AddChild(normal, 0);
        SetSize(m_normal->GetSize());
        m_pressDetecter->SetSize(normal->GetSize());
        m_normalStartPos = m_normal->GetPosition();
    }
	return this;
}

CButton* CButton::SetPressed(CSprite* pressed,bool delActions)
{
    if (delActions)
        RemoveButtonActions();
    
	if (m_pressed) RemoveChild(m_pressed, true);
	m_pressed = pressed;
    if (m_pressed)
    {
        AddChild(pressed);
        pressed->SetVisible(false);
        pressed->SetPause(true);
        SetSmoothSwich(m_swichTime);
        m_pressedShift = pressed->GetPosition();
        m_pressedStartPos = m_pressed->GetPosition();
    }
	return this;
}

CButton* CButton::SetDisabled(CSprite* disabled,bool delActions)
{
	if (m_disable) RemoveChild(m_disable, true);
	m_disable = disabled;
    if (m_disable)
    {
        AddChild(m_disable);
        m_disable->SetPause(true);
        m_disable->SetVisible(false);
    }
	return this;
}

CButton* CButton::SetPressedShift(vec2 shift)
{
	m_shift = shift;
	return this;
}

CButton* CButton::SetPressedScale(vec2 scale)
{
	m_scaleshift = scale;
	return this;
}

CButton* CButton::SetOnOverButton(bool flag)
{
    m_OnOverButton = flag;
    return this;
}

CButton* CButton::SetPressedRectScale(vec2 scale)
{
	m_pressDetecter->SetSize(vec2(GetSize().x * scale.x, GetSize().y * scale.y));
	return this;
}

CButton* CButton::SetPressedRectSize(vec2 size)
{
    m_pressDetecter->SetSize(size);
	return this;
}

CButton* CButton::SetPressedRectShift(vec2 shift)
{
    if (!m_normal) return this;
    m_pressDetecter->SetPosition(shift);
    return this;
}

CButton* CButton::SetPressedRectVisible(float r, float g, float b)
{
    m_pressDetecter->SetColor(sColor(r, g, b, 0.3f));
    m_pressDetecter->SetAnchorPoint(vec2(0.5f, 0.5f));
    return this;
}

CButton* CButton::SetPressedScale(float scale)
{
	m_scaleshift = vec2(scale, scale);
	return this;
}

CButton* CButton::SetPressedRotate(float angle)
{
    m_angle = angle;
    return this;
}

CButton* CButton::SetSmoothSwich(float time)
{
	m_swichTime = time;
	return this;
}

CButton* CButton::SetEnable(bool isEnable)
{
    if (isEnable == m_isEnable) return this;
	m_isEnable = isEnable;
	Unselect();
	
	if (m_disable)
	{
		if (!m_isEnable)
		{
			m_disable->SetPause(false);
			m_disable->SetVisible(true);
			if (m_normal)  m_normal->SetPause(true);
			if (m_normal)  m_normal->SetVisible(false);
			if (m_pressed) m_pressed->SetPause(true);
			if (m_pressed) m_pressed->SetVisible(false);
		}
		else 
		{
			if (m_disable) m_disable->SetPause(true);
			if (m_disable) m_disable->SetVisible(false);
			if (m_normal)  m_normal->SetPause(false);
			if (m_normal)  m_normal->SetVisible(true);
		}
	}	
	
	return this;
}

CSprite* CButton::SetSize(const vec2& size)
{
    CSprite::SetSize(size);
    m_pressDetecter->SetSize(size);
    return this;
}
CSprite* CButton::SetAnchorPoint(const vec2& anchorPoint)
{
    CSprite::SetAnchorPoint(anchorPoint);
    if (m_normal) m_normal->SetAnchorPoint(anchorPoint);
    if (m_pressed) m_pressed->SetAnchorPoint(anchorPoint);
    if (m_disable) m_disable->SetAnchorPoint(anchorPoint);
    if (m_pressDetecter) m_pressDetecter->SetAnchorPoint(anchorPoint);
    return this;
}

CSprite* CButton::SetIsAlphaIgnore(bool isIgnor)
{
    if (isIgnor && !m_alphaTouch && m_normal)
    {        
        int width;
        int height;
        sColor* points = NULL;//m_normal->CreateSpriteTextRect(width, height);
        if(points != NULL)
        {
            m_alphaTouch = new float[width * height];
            m_alphaSize.width = width;
            m_alphaSize.height = height;
            for (int i = 0; i < width * height; i++)
                m_alphaTouch[i] = points[i].sAlpha;
            delete[] points;
        }
    }
    else if (!isIgnor && m_alphaTouch)
    {
        delete[] m_alphaTouch;
        m_alphaTouch = NULL;
    }    
    
    return this;
}

CSprite* CButton::GetNormal()
{
    return m_normal;
}

CSprite* CButton::GetPressed()
{
    return m_pressed;
}

CSprite* CButton::GetDisabled()
{
    return m_disable;
}

CEvent& CButton::GetEventOnSelect()
{
	return m_selectEvent;
}
CEvent& CButton::GetEventOnUnSelect()
{
	return m_unSelectEvent;
}

CEvent& CButton::GetEventOnPress()
{
	return m_pressEvent;
}

vec2 CButton::GetPressedRectSize()
{
    return m_pressDetecter->GetSize();
}
vec2 CButton::GetPressedRectShift()
{
    return m_pressDetecter->GetPosition();
}

CButton* CButton::SetData(void* data)
{
    m_eventData = data;
    return this;
}
void* CButton::GetData()
{
    return m_eventData;
}
CNode* CButton::SetIsTouchesOn(bool isTouch)
{
    Unselect();
    return CNode::SetIsTouchesOn(isTouch);
}
void CButton::RemoveButtonActions()
{
    SAFE_DELETE_ACTION(m_normalScaleAnim)
    SAFE_DELETE_ACTION(m_procActive)
    SAFE_DELETE_ACTION(m_procNormal)

    SetIsPressed(m_isSelect);
}

void CButton::RemoveButtonEvents()
{
    m_selectEvent.RemoveAllListeners();
    m_unSelectEvent.RemoveAllListeners();
    m_pressEvent.RemoveAllListeners();
}

bool CButton::TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed)
{
	if (!m_normal || !m_isEnable || isPrecessed || !IsVisible(false)) return isPrecessed;
		
	if (DetectTouch(pos))
	{
		m_tochID = ID;
		m_isSelect = true;
		      
        if (m_isNormalScaleAnim)
            RemoveNormalScaleAnim();
        
		if (m_swichTime != 0)
		{
            if (m_procNormal)
                m_procNormal->Clear();
            
            if (m_procActive == NULL)
                AddAction(m_procActive = new CActionSequence(false));

            if (m_procActive->IsEmpty())
			{
                if (m_pressed)
                {
                    m_pressed->SetActive(true);
                    m_pressed->SetOpacity(0.0);
                    m_pressed->SetPosition(m_pressedShift);
                    m_pressed->SetScale(1);
                }
                
                m_normal->SetPosition(m_normalStartPos);
                m_normal->SetScale(1);
                
                if (!m_pressed) 
                    m_pressed = m_pressDetecter;
                
				m_procActive->Add(new CActionSimultaneously(
									ActionChangeOpacity(m_pressed, 0, 1, m_swichTime),
									//ActionMoveBy(m_normal, m_shift, m_swichTime),
									//ActionMoveBy(m_pressed, m_shift, m_swichTime),
                                    ActionMoveTo(m_normal, m_normalStartPos + m_shift, m_swichTime),
                                    ActionMoveTo(m_pressed, m_pressedStartPos + m_shift, m_swichTime),
									ActionChangeScaleTo(m_normal, m_scaleshift, m_swichTime),
									ActionChangeScaleTo(m_pressed, m_scaleshift, m_swichTime),
                                    ActionRotateTo(m_normal, m_angle, m_swichTime), 
                                    ActionRotateTo(m_pressed, m_angle, m_swichTime),
									NULL));
                if (m_pressed == m_pressDetecter) 
                    m_pressed = NULL;
			}
		}
		else 
			SetIsPressed(true);
		
        
        if (m_isSelect) 
		{
			SButtonEventData data(this, SButtonEventData::etSelect, pos, m_eventData);
			m_selectEvent.RunEvent(&data);
		}
        
		return true;
	}
	
	return isPrecessed;
}

bool CButton::TouchMove(intptr_t ID, vec2& pos, bool isPrecessed)
{
	if (!m_normal || !m_isEnable /*|| isPrecessed*/ || !IsVisible(false)) return isPrecessed;
	
	if (!m_isSelect && m_OnOverButton)
	{
		CButton::TouchBegin(ID, pos, isPrecessed);
	}
		
	if (m_tochID != ID) return isPrecessed;

	if (!DetectTouch(pos) || isPrecessed)
	{
        Unselect();
		
        SButtonEventData data(this, SButtonEventData::etUnselect, pos, m_eventData);
        m_unSelectEvent.RunEvent(&data);
        
		return isPrecessed;
	}	
	
	return true;
}

bool CButton::TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed)
{
	if (!m_normal || !m_isEnable || isPrecessed ||!IsVisible(false)) return isPrecessed;
    
	if (m_tochID != ID) return isPrecessed;
    

    SButtonEventData data(this, SButtonEventData::etPress, pos, m_eventData);
    
    if (m_isSelect) m_unSelectEvent.RunEvent(&data);
	Unselect();
    if (DetectTouch(pos))
	{
		m_pressEvent.RunEvent(&data);
		return true;
	}
	
	return isPrecessed;
}

void CButton::TouchCancel(intptr_t ID, vec2& pos)
{
    
    if (m_tochID == ID)
    {
        SButtonEventData data(this, SButtonEventData::etPress, pos, m_eventData);
        m_unSelectEvent.RunEvent(&data);
        Unselect();
    }

}

void CButton::TouchReset(intptr_t ID, vec2& pos)
{
    TouchCancel(ID, pos);
}

void CButton::Unselect()
{
    m_isSelect = false;
    m_tochID = 0;
    
    if (m_isNormalScaleAnim)
        AddNormalScaleAnim();
    
    if (m_swichTime != 0)
    {
        m_normal->SetPause(false);
        m_normal->SetVisible(true);
        
        if (!m_pressed) 
            m_pressed = m_pressDetecter;
        
        if (m_procActive)
            m_procActive->Clear();

        if (m_procNormal == NULL)
            AddAction(m_procNormal = new CActionSequence(false));
        else
            m_procNormal->Clear();
            
        
        m_procNormal->Add(new CActionSimultaneously(
                                                   ActionChangeOpacity(m_pressed, 1, 0, m_swichTime),
                                                   ActionMoveTo(m_normal, m_normalStartPos, m_swichTime),
                                                   ActionMoveTo(m_pressed, m_pressedStartPos, m_swichTime),
                                                   ActionChangeScaleTo(m_normal, 1, m_swichTime),
                                                   ActionChangeScaleTo(m_pressed, 1, m_swichTime),
                                                   ActionRotateTo(m_normal, 0, m_swichTime),
                                                   ActionRotateTo(m_pressed, 0, m_swichTime),
                                                   NULL));
                         

        if (m_pressed == m_pressDetecter) 
            m_pressed = NULL;       
    }
    else 
        SetIsPressed(false);
    
}


void CButton::SetNormalScaleAnim(bool value)
{
    m_isNormalScaleAnim = value;
    if (!m_isSelect)
    {
        if (m_isNormalScaleAnim)
            AddNormalScaleAnim();
        else
            RemoveNormalScaleAnim();
    }
}

void CButton::CheckPressedRect()
{
    if (m_normal)
    {
        NODE_LOOP(m_normal, it)
        {
            if((*it)->GetName() == "touch_rect")
            {
                CSprite* touch_rect = (CSprite*)*it;
                SetPressedRectSize(touch_rect->GetSize());
                SetPressedRectShift(touch_rect->GetPosition());
                touch_rect->SetActive(false);
            }
        }
    }
}

void CButton::AddNormalScaleAnim()
{
    SAFE_DELETE_ACTION(m_normalScaleAnim)

    m_normalScaleAnim = new CActionRepeate(new CActionSequence(ActionChangeScale(m_normal, 1.0, 1.05, 0.25f),
                                                               ActionChangeScale(m_normal, 1.05, 0.95, 0.5f),
                                                               ActionChangeScale(m_normal, 0.95, 1.0, 0.25f),
                                                               NULL));
    m_normal->AddAction(m_normalScaleAnim);
}

void CButton::RemoveNormalScaleAnim()
{
    if(m_normalScaleAnim)
    {
        m_normalScaleAnim->Delete();
        m_normalScaleAnim = NULL;
    }
    m_normal->SetScale(1.0);
}

void CButton::SetIsPressed(bool isPressed)
{	
	if (m_pressed)
    {
        m_pressed->SetPause(!isPressed);
        m_pressed->SetVisible(isPressed);
        m_normal->SetPause(isPressed);
        m_normal->SetVisible(!isPressed);       
        m_pressed->SetPosition((isPressed) ? m_pressedShift+m_shift : m_pressedShift);
        m_pressed->SetScale((isPressed) ? m_scaleshift : vec2(1, 1));
        m_pressed->SetAngle((isPressed) ? m_angle : 0);
    }
    
    if (m_normal)
    {
        m_normal->SetPosition((isPressed) ? m_normalStartPos + m_shift : m_normalStartPos);
        m_normal->SetScale((isPressed) ? m_scaleshift : vec2(1, 1));
        m_normal->SetAngle((isPressed) ? m_angle : 0);
    }
}

bool CButton::DetectTouch(vec2 pos)
{
    if (!m_alphaTouch)
        return m_pressDetecter->IsPointInSprite(pos);
    else if (m_normal->IsPointInSprite(pos))
    {
        vec2 locpos = m_normal->GetLocalFromGlobalCoord(pos);
        locpos += vec2(m_normal->GetSize().x * m_normal->GetAnchorPoint().x, 
                       m_normal->GetSize().y * m_normal->GetAnchorPoint().y);
        locpos.y = m_normal->GetSize().y - locpos.y;
        int x = locpos.x * m_alphaSize.width / m_normal->GetSize().width;
        int y = locpos.y * m_alphaSize.height / m_normal->GetSize().height;
        if (y < m_alphaSize.height && x < m_alphaSize.width)
            return (m_alphaTouch[y * (int)m_alphaSize.width + x] != 0);
        return false;
    }
    else
        return false;
}

CButton* CButton::GetCopy()
{
	return new CButton(*this);
}

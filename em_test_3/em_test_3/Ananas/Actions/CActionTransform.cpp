//
//  CActionTransform.cpp
//  House_of_Fun_2_10
//
//  Created by RVV on 03.12.14.
//  Copyright (c) 2014 PacificInteractive. All rights reserved.
//

#include "CActionTransform.h"
#include "CFile.h"
#include "AEInterpolation.h"

CActionTransform::CActionTransform(CSprite* object, const AETransform::STData& m_transform, float duration, float speed)
:m_object(object)
,m_duration(duration)
,m_speed(speed)
,m_transform(m_transform)
,m_curTime(0.0f)
,m_isRevers(false)
,m_isLoop(false)
,m_shiftAncr(vec2())
,m_shiftPos(vec2())
,m_shiftScale(vec2(1.0f, 1.0f))
,m_shiftAngle(0.0f)
,m_shiftOpacity(1.0f)
{
    AddLink(m_object);
}


CActionTransform::CActionTransform(CSprite* object, const pugi::xml_node& nodeTransform)
:m_object(object)
,m_duration(-1.0f)
,m_speed(1.f)
,m_transform(AETransform::STData())
,m_curTime(0.0f)
,m_isRevers(false)
,m_isLoop(false)
,m_shiftAncr(vec2())
,m_shiftPos(vec2())
,m_shiftScale(vec2(1.0f, 1.0f))
,m_shiftAngle(0.0f)
,m_shiftOpacity(1.0f)
{
    AddLink(m_object);
    
    ParseNodeTransform(nodeTransform);
}

CActionTransform::CActionTransform(CSprite* object, const std::string& xmlTransform):
m_object(object)
,m_duration(-1.0f)
,m_speed(1.f)
,m_transform(AETransform::STData())
,m_curTime(0.0f)
,m_isRevers(false)
,m_isLoop(false)
,m_shiftAncr(vec2())
,m_shiftPos(vec2())
,m_shiftScale(vec2(1.0f, 1.0f))
,m_shiftAngle(0.0f)
,m_shiftOpacity(1.0f)
{
    AddLink(m_object);
    
    pugi::xml_document m_xmlDoc;

    CFile file(xmlTransform);
    if(!file.IsExist())
    {
        ALOG("File %s is not exist\n", xmlTransform.c_str());
        return;
    }
    
    pugi::xml_parse_result parseResult = m_xmlDoc.load(file.Read().data());
    
    if (parseResult.status != pugi::status_ok)
    {
        ALOG("Error CActionTransform: can't load xml %s\n", xmlTransform.c_str());
        return;
    }
    
    pugi::xml_node nodeTransform(m_xmlDoc.child("transform"));
    if (!nodeTransform)
    {
        ALOG("Error CActionTransform: node transform is missing");
        return;
    }
    
    ParseNodeTransform(nodeTransform);
}

CActionTransform::CActionTransform(const CActionTransform& action)
:m_object(action.m_object)
,m_duration(action.m_duration)
,m_speed(action.m_speed)
,m_transform(action.m_transform)
,m_curTime(0.0f)
,m_isRevers(false)
,m_isLoop(action.m_isLoop)
,m_shiftAncr(action.m_shiftAncr)
,m_shiftPos(action.m_shiftPos)
,m_shiftScale(action.m_shiftScale)
,m_shiftAngle(action.m_shiftAngle)
,m_shiftOpacity(action.m_shiftOpacity)
{
    Revers(action.m_isRevers);
}

CActionTransform::~CActionTransform()
{
    
}

void CActionTransform::ParseNodeTransform(const pugi::xml_node &nodeTransform)
{
    m_duration = nodeTransform.attribute("duration").as_float();
    
    if (nodeTransform.attribute("speed"))
        m_speed = nodeTransform.attribute("speed").as_float();

    if (nodeTransform.child("transformAnchorPoint"))
        m_transform.m_AnchorPoint = new AEInterpolationSaptial(nodeTransform.child("transformAnchorPoint"));

    if (nodeTransform.child("transformPosition"))
        m_transform.m_Position = new AEInterpolationSaptial(nodeTransform.child("transformPosition"));

    if (nodeTransform.child("transformScale"))
        m_transform.m_Scale = AEInterpolation::ParseTransform(nodeTransform.child("transformScale"), 2);

    if (nodeTransform.child("transformRotation"))
        m_transform.m_Rotate = AEInterpolation::ParseTransform(nodeTransform.child("transformRotation"), 1);
    
    if (nodeTransform.child("transformOpacity"))
        m_transform.m_Opacity = AEInterpolation::ParseTransform(nodeTransform.child("transformOpacity"), 1);
}

CAction* CActionTransform::GetReversCopy()
{
    CActionTransform* action(new CActionTransform(*this));
    action->Revers(true);
    return action;
}

void CActionTransform::Update(float dt)
{
    m_curTime +=  m_isRevers ? -dt * m_speed : dt * m_speed;
    
    //transform
    if (m_transform.m_AnchorPoint)
        m_object->SetAnchorPoint(m_shiftAncr + m_transform.m_AnchorPoint->GetVec2(m_curTime));
    if (m_transform.m_Position)
        m_object->SetPosition(m_shiftPos + m_transform.m_Position->GetVec2(m_curTime));
    if (m_transform.m_Scale)
        m_object->SetScale(prod(m_shiftScale, m_transform.m_Scale->GetVec2(m_curTime)));
    if (m_transform.m_Rotate)
        m_object->SetAngle(m_shiftAngle + m_transform.m_Rotate->Get(m_curTime));
    if (m_transform.m_Opacity)
        m_object->SetOpacity(m_shiftOpacity * m_transform.m_Opacity->Get(m_curTime));
    //end transform
    
    if (!m_isLoop)
    {
        if (m_isRevers && m_curTime <= 0.0f)
            Delete();
        else if(!m_isRevers && m_curTime >= m_duration)
            Delete();
    }
    else
    {
        if (m_isRevers && m_curTime <= 0.0f)
            m_curTime = m_duration;
        else if (m_curTime >= m_duration)
            m_curTime = 0.f;

    }
}
void CActionTransform::Loop(bool isLoop)
{
    m_isLoop = isLoop;
}

void CActionTransform::Revers(bool isEnable)
{
    m_isRevers = isEnable;
    if (m_isRevers)
        m_curTime = m_duration;
    else
        m_curTime = 0.0f;
}


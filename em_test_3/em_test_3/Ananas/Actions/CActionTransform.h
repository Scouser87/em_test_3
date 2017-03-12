//
//  CActionTransform.h
//  House_of_Fun_2_10
//
//  Created by RVV on 03.12.14.
//  Copyright (c) 2014 PacificInteractive. All rights reserved.
//

/*
 Экшн изменяет параметры CSprite согласно трансформаций описанным в XML.
 Описание трансформаций XML-формате, возможно получить с помощью скрипта ConverterAfterEffects.jsx
 Если какие то трансформации не нужны, их целесообразно удалить с XML. Стоит обратить особое внимание 
 на "transformAnchorPoint", поскольку в AfterEffects anchor point работает по другому.
 
 ВНИМАНИЕ! Является обязательным атрибут "duration" в теге "transform".
 duration - указывается длительность работы экшена
 
 Cтруктура XML:
 <transform duration="3">
    <transformAnchorPoint type="0" value="0.5,0.5"/>
    <transformPosition type="2" value="-19,-7">
        <key time="0" value="-19,-7"/>
        <key time="2" value="176,-139"/>
        <key time="2.98333333333333" value="29,-56"/>
    </transformPosition>
    <transformScale type="1" value="1.12089728453365,1">
        ...
    </transformScale>
    <transformRotation type="1" value="0">
        ...
    </transformRotation>
    <transformOpacity type="1" value="1">
        ...
 </transform>
 */
#pragma once

#include "CAction.h"
#include "../GraphicsLoader/AfterEffects/AETransform.h"


class CActionTransform: public CAction
{
public:
    CActionTransform(CSprite* object, const AETransform::STData& m_transform, float duration, float speed = 1.f);
    CActionTransform(CSprite* object, const pugi::xml_node& nodeTransform);
    CActionTransform(CSprite* object, const std::string& xmlTransform);
    CActionTransform(const CActionTransform& action);
	~CActionTransform();
//---  Даная група методов, корректно работает только до начала работы экшена!
    CAction*	GetCopy()         {return new CActionTransform(*this);}
    CAction*	GetReversCopy();
    //при изменении параметров CSprite, учитываются начальные значения. Расчет приведен в комментариях к методу
    void UseAnchorPointOriginal(){m_shiftAncr    = m_object->GetAnchorPoint();} // originalAncr + transformAncr
    void UsePositionOriginal()   {m_shiftPos     = m_object->GetPosition();}    // originalPos + transformPos
    void UseScaleOriginal()      {m_shiftScale   = m_object->GetScale();}       // originalScale * transformScale
    void UseOpacityOriginal()    {m_shiftOpacity = m_object->GetOpacity();}     // originalOpacity * transformOpacity
    void UseAngleOriginal()      {m_shiftAngle   = m_object->GetAngle();}       // originalAngle + transformAngle
    
    void Revers(bool isEnable);
    void Loop(bool isLoop);
//---
    
    float		GetDurationTime() {return m_duration * m_speed;}
    void		Update(float dt);

    
private:
    CSprite*            m_object;
    float               m_duration;
    AETransform::STData m_transform;
    
    float   m_curTime;
    bool    m_isRevers;
    bool    m_isLoop;
    
    vec2    m_shiftAncr;
    vec2    m_shiftPos;
    vec2    m_shiftScale;
    float   m_shiftAngle;
    float   m_shiftOpacity;
    float   m_speed;
    
    void ParseNodeTransform(const pugi::xml_node& nodeTransform);
};

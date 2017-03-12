/*
 *  CActionScaleChange.cpp
 *  Casino
 *
 *  Created by Anton Zagovalko on 02.02.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionModifiers.h"

#include "../sound/new/BaseSound.h"

CAction* ActionMove(CSprite* node, vec2 from, vec2 to, float time, float speed)
{
	return new CActionParameterChange< vec2, CSprite >(node, &CSprite::SetPosition, from, to, time, speed);
}

CAction* ActionMoveTo(CSprite* node, vec2 to, float time, float speed)
{
	return CActionParameterChange< vec2, CSprite >::To(node, &CSprite::SetPosition, &CSprite::GetPosition, to, time, speed);
}

CAction* ActionMoveBy(CSprite* node, vec2 to, float time, float speed)
{
	return CActionParameterChange< vec2, CSprite >::By(node, &CSprite::SetPosition, &CSprite::GetPosition, to, time, speed);
}

CAction* ActionMoveSpline(CSprite* node, CSpline* spline, float time, bool first_pos, bool loop)
{
    return new CActionMoveSpline <CSprite> (node, &CSprite::SetPosition, spline, time, first_pos,loop);
}

CAction* ActionChangeOpacity(CSprite* node, float from, float to, float time, float speed)
{
	return new CActionParameterChange< float, CSprite >(node, &CSprite::SetOpacity, from, to, time, speed);
}

CAction* ActionChangeOpacityTo(CSprite* node, float to, float time, float speed)
{
	return CActionParameterChange< float, CSprite >::To(node, &CSprite::SetOpacity, &CSprite::GetOpacity, to, time, speed);
}

CAction* ActionChangeOpacityBy(CSprite* node, float to, float time, float speed)
{
	return CActionParameterChange< float, CSprite >::By(node, &CSprite::SetOpacity, &CSprite::GetOpacity, to, time, speed);
}

CAction* ActionAnchorPointTo(CSprite* node, vec2 to, float time, float speed)
{
    return CActionParameterChange< vec2, CSprite >::To(node, &CSprite::SetAnchorPoint, &CSprite::GetAnchorPoint, to, time, speed);
}



CAction* ActionChangeScale(CSprite* node, vec2 from, vec2 to, float time, float speed)
{
	return new CActionParameterChange< vec2, CSprite >(node, &CSprite::SetScale, from, to, time, speed);
}

CAction* ActionChangeScale(CSprite* node, float from, float to, float time, float speed)
{
	return new CActionParameterChange< vec2, CSprite >(node, &CSprite::SetScale, vec2(from, from), vec2(to, to), time, speed);
}

CAction* ActionChangeScaleTo(CSprite* node, vec2 to, float time, float speed)
{
	return CActionParameterChange< vec2, CSprite >::To(node, &CSprite::SetScale, &CSprite::GetScale, to, time, speed);
}

CAction* ActionChangeScaleTo(CSprite* node, float to, float time, float speed)
{
	return CActionParameterChange< vec2, CSprite >::To(node, &CSprite::SetScale, &CSprite::GetScale, vec2(to, to), time, speed);
}

CAction* ActionChangeScaleBy(CSprite* node, vec2 to, float time, float speed)
{
	return CActionParameterChange< vec2, CSprite >::By(node, &CSprite::SetScale, &CSprite::GetScale, to, time, speed);
}

CAction* ActionChangeScaleBy(CSprite* node, float to, float time, float speed)
{
	return CActionParameterChange< vec2, CSprite >::By(node, &CSprite::SetScale, &CSprite::GetScale, vec2(to, to), time, speed);
}



CAction* ActionChangeSize(CSprite* node, vec2 from, vec2 to, float time, float speed)
{
    return new CActionParameterChange< vec2, CSprite >(node, &CSprite::SetSize, from, to, time, speed);
}
CAction* ActionChangeSizeTo(CSprite* node, vec2 to, float time, float speed)
{
    return CActionParameterChange< vec2, CSprite >::To(node, &CSprite::SetSize, &CSprite::GetSize, to, time, speed);
}
CAction* ActionChangeSizeBy(CSprite* node, vec2 to, float time, float speed)
{
    return CActionParameterChange< vec2, CSprite >::By(node, &CSprite::SetSize, &CSprite::GetSize, to, time, speed);
}


CAction* ActionRotate(CSprite* node, float from, float to, float time, float speed)
{
	return new CActionParameterChange< float, CSprite >(node, &CSprite::SetAngle, from, to, time, speed);
}

CAction* ActionRotateTo(CSprite* node, float to, float time, float speed)
{
	return CActionParameterChange< float, CSprite >::To(node, &CSprite::SetAngle, &CSprite::GetAngle, to, time, speed);
}

CAction* ActionRotateBy(CSprite* node, float to, float time, float speed)
{
	return CActionParameterChange< float, CSprite >::By(node, &CSprite::SetAngle, &CSprite::GetAngle, to, time, speed);
}




CAction* ActionChangeColor(CSprite* node, sColor from, sColor to, float time)
{
	return new CActionParameterChange< sColor, CSprite >(node, &CSprite::SetColor, from, to, time);
}

CAction* ActionChangeColorTo(CSprite* node, sColor to, float time)
{
	return CActionParameterChange< sColor, CSprite >::To(node, &CSprite::SetColor, &CSprite::GetColor, to, time);
}

CAction* ActionChangeColorBy(CSprite* node, sColor to, float time)
{
	return CActionParameterChange< sColor, CSprite >::By(node, &CSprite::SetColor, &CSprite::GetColor, to, time);

}




#if (OS != OS_MAC)
CAction* ActionChangeVolume(CBaseSound* node, float from, float to, float time, float speed)
{
	return new CActionParameterChange< float, CBaseSound >(node, &CBaseSound::SetVolume, from, to, time, speed);
}

CAction* ActionChangeVolumeTo(CBaseSound* node, float to, float time, float speed)
{
	return CActionParameterChange< float, CBaseSound >::To(node, &CBaseSound::SetVolume, &CBaseSound::GetVolume, to, time, speed);
}

CAction* ActionChangeVolumeBy(CBaseSound* node, float to, float time, float speed)
{
	return CActionParameterChange< float, CBaseSound >::By(node, &CBaseSound::SetVolume, &CBaseSound::GetVolume, to, time, speed);
}
#endif

/*
 *  CActionScaleChange.h
 *  Casino
 *
 *  Created by Anton Zagovalko on 02.02.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CAction.h"

/////////////////////////  экшен для перемещения /////////////
CAction* ActionMove(CSprite* node, vec2 from, vec2 to, float time, float speed = 0);	// менять из from в to
CAction* ActionMoveTo(CSprite* node, vec2 to, float time, float speed = 0);				// менять в to 
CAction* ActionMoveBy(CSprite* node, vec2 to, float time, float speed = 0);				// менять относительно текущено на to

CAction* ActionMoveSpline(CSprite* node, CSpline* spline, float time, bool first_pos = true,bool loop=false);                  // менять по сплайну

CAction* ActionAnchorPointTo(CSprite* node, vec2 to, float time, float speed = 0);          // менять в to

/////////////////////////  экшен для изменения непрозрачности /////////////
CAction* ActionChangeOpacity(CSprite* node, float from, float to, float time, float speed = 0);		// менять из from в to
CAction* ActionChangeOpacityTo(CSprite* node, float to, float time, float speed = 0);				// менять в to 
CAction* ActionChangeOpacityBy(CSprite* node, float to, float time, float speed = 0);				// менять относительно текущено на to

/////////////////////////  экшен для изменения скейла /////////////
CAction* ActionChangeScale(CSprite* node, vec2 from, vec2 to, float time, float speed = 0);		// менять из from в to
CAction* ActionChangeScale(CSprite* node, float from, float to, float time, float speed = 0);	//
CAction* ActionChangeScaleTo(CSprite* node, vec2 to, float time, float speed = 0);				// менять в to 
CAction* ActionChangeScaleTo(CSprite* node, float to, float time, float speed = 0);				// 
CAction* ActionChangeScaleBy(CSprite* node, vec2 to, float time, float speed = 0);				// менять относительно текущено на to
CAction* ActionChangeScaleBy(CSprite* node, float to, float time, float speed = 0);				//

/////////////////////////  экшен для изменения Size /////////////
CAction* ActionChangeSize(CSprite* node, vec2 from, vec2 to, float time, float speed = 0);		// менять из from в to
CAction* ActionChangeSizeTo(CSprite* node, vec2 to, float time, float speed = 0);				// менять в to
CAction* ActionChangeSizeBy(CSprite* node, vec2 to, float time, float speed = 0);				// менять относительно текущено на to

///////////////////////// экшен для изменения угла //////////////////
CAction* ActionRotate(CSprite* node, float from, float to, float time, float speed = 0);	// менять из from в to
CAction* ActionRotateTo(CSprite* node, float to, float time, float speed = 0);				// менять в to 
CAction* ActionRotateBy(CSprite* node, float to, float time, float speed = 0);				// менять относительно текущено на to

///////////////////////// экшен для изменения цвета //////////////////
CAction* ActionChangeColor(CSprite* node, sColor from, sColor to, float time);		// менять из from в to
CAction* ActionChangeColorTo(CSprite* node, sColor to, float time);					// менять в to 
CAction* ActionChangeColorBy(CSprite* node, sColor to, float time);					// менять относительно текущено на to


/////////////////////////  экшен для изменения громкости /////////////

#if (OS != OS_MAC)
class CBaseSound;
CAction* ActionChangeVolume(CBaseSound* node, float from, float to, float time, float speed = 0);   // менять из from в to
CAction* ActionChangeVolumeTo(CBaseSound* node, float to, float time, float speed = 0);				// менять в to
CAction* ActionChangeVolumeBy(CBaseSound* node, float to, float time, float speed = 0);				// менять относительно текущено на to
#endif

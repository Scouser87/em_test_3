/*
 *  CActonAcceleration.h
 *  Ananas
 *
 *  Created by Anton Zagovalko on 30.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "CAction.h"

// изменяет течение времени в добвленном в него экшене
class CActionAcceleration: public CAction
{
public:
                        // coefStart - коэффициент течения времени вначале coefEnd - в конце, 
                        // time = время за которое изменяется коефициете (если 0 - берется время выполнения экшена)
    CActionAcceleration(CAction* act, float coefStart, float coefEnd, float time = 0);	
    ~CActionAcceleration();
	void		Update(float dt);
	CAction*	GetCopy();
	CAction*	GetReversCopy();
	float		GetDurationTime();
    
private:
    typedef CActionParameterChange< float, CActionAcceleration > ChangerType;
    
    CAction*	 m_action;
    float        m_curCoef;
    ChangerType* m_changer;
    
    CActionAcceleration* SetCoef(const float& coef);
    friend class CActionParameterChange< float, CActionAcceleration >;
};


// выполняет действие с ускорением при этом сохраняя время его воспроизведения
class CActionAccelerationTimeSave: public CAction
{
public:
				CActionAccelerationTimeSave(CAction* act, float coef);	// >0 изменение с ускорением <0 с замедлением. 0, 1, -1 - равномерное движение
				~CActionAccelerationTimeSave();
	void		Update(float dt);
	CAction*	GetCopy();
	CAction*	GetReversCopy();
	float		GetDurationTime();
	
private:
	float		TimeFunc(float);
	
	CAction*	m_action;
	float		m_coef;
	float		m_time;
	float		m_prevTime;
	float		m_timeMax;
	
};

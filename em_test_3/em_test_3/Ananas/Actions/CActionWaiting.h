/*
 *  CActionWaiting.h
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CAction.h"

class CActionWaiting : public CAction
{
public:
				CActionWaiting(float time);		// временная задержка time секунд
	void		Update(float dt);
	CAction*	GetCopy();
	float		GetDurationTime();
	float		GetTime();
    void        AddTime(float time);
    void        SetTime(float time);
	
private:
	float		m_timeMax;
	float		m_curTime;
};


class CActionWaitingFor : public CAction
{
public:
	template<class T>	CActionWaitingFor(CNode* obj, bool (T::*func)(void))	/* ожидает пока функция func не вернет true (она вызывается каждый кадр) */ : m_action(NULL), m_node(obj), m_func((bool (CNode::*)(void))func) {}
						CActionWaitingFor(CAction* act);		// ожидает завершения экшена (работает только с экшенами добавленными в менеджер, а не в другой экшен)
	void				Update(float dt);
	CAction*			GetCopy();
	float				GetDurationTime();
	
private:
	bool (CNode::*m_func)(void);
    CNode*   m_node;
	CAction* m_action;
};
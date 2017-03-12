/*
 *  CActionSimultaneously.h
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CAction.h"

class CActionSimultaneously: public CAction
{
public:
				CActionSimultaneously(CAction* first, CAction* next, ...);  // последний элемент должен быть NULL
                CActionSimultaneously(const std::vector< CAction* >& els);
                CActionSimultaneously(const CActionSimultaneously& action);
				~CActionSimultaneously();
    void        CheckForLinks();    // копируем линки на обьекты CNode, прилинкованные к Action-ам из списка m_actions
	void		Update(float dt);
	CAction*	GetCopy();
    CAction*	GetReversCopy();
    CAction*	Add(CAction* act);  // добавляет экшн в список
	float		GetDurationTime();

private:
	std::list< CAction* > m_actions;
	
	void RemoveAction(CAction* action);
};
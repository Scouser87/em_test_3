/*
 *  CActionSequence.h
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CAction.h"

class CActionSequence: public CAction
{
public:
                CActionSequence(bool autodelete = true);  //для добавления через функцию Add
				CActionSequence(CAction* first, CAction* next, ...);  // последний элемент должен быть NULL
                CActionSequence(const std::vector< CAction* >& els);
                CActionSequence(const CActionSequence& action);
				~CActionSequence();
    void        CheckForLinks();    // копируем линки на обьекты CNode, прилинкованные к Action-ам из списка m_actions
	void		Update(float dt);
	CAction*	GetCopy();
	CAction*	GetReversCopy();
	CAction*	Add(CAction* act, int position = -1);
    void        Clear();
    bool        IsEmpty(){return m_actions.empty();}
	float		GetDurationTime();
    
private:
    void RemoveAction(CAction *action);
	
private:
	std::vector< CAction* > m_actions;
    std::vector< CAction* > m_delete_buffer;
	int m_curAction;
    bool m_autoDelete;
    bool m_inUpdate;
};



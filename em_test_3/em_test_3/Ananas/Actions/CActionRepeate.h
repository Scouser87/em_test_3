/*
 *  CActionRepeate.h
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "CAction.h"

///////////////// CActionRepeate //////////////////
class CActionRepeate: public CAction
{
public:
				CActionRepeate(CAction* action, int count = -1); // если count <= 0 то экшен будет выполсяться бесконечно
				CActionRepeate(const CActionRepeate& action);
                ~CActionRepeate();
	void		Update(float dt);
	CAction*	GetCopy();
	float		GetDurationTime();
	
private:
	CAction* m_action;
	CAction* m_actionCopy;
	int m_countMax;
	int m_counter;	
};
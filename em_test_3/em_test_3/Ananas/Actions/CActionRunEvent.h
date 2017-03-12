/*
 *  CActionRunEvent.h
 *  Casino
 *
 *  Created by Anton Zagovalko on 30.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "CAction.h"
#include "CActionRepeate.h"
#include "CActionSequence.h"
#include "CActionWaiting.h"
#include "../CDirector.h"

//////////////// екшен запуска евента ////////////////////
class CActionRunEvent: public CAction
{
public:
	CActionRunEvent();                      // создает объект с новым евентом, который можно получить через GetEvent()
	CActionRunEvent(const CEvent& ev);		// создает объект с заданным евентом
    CActionRunEvent(const CActionRunEvent& act);
    ~CActionRunEvent();
    
	template<class T>	CActionRunEvent(CNode* obj, void (T::*handler)(void*))	/* задает обработчик который будет запущен */		: m_data(NULL) { m_obj = obj; obj->AddEventHandler(m_event, handler); AddLink(obj);}
    
    CActionRunEvent(CNode* obj, std::function<void(void*)> handler) : m_data(NULL) { m_obj = obj; obj->AddEventHandler(m_event, obj, handler); }

	CActionRunEvent*	SetData(void* data);
	
	void				Update(float dt);
	CAction*			GetCopy();
	float				GetDurationTime();
	
private:
	CEvent	m_event;
	void*	m_data;
    CNode*  m_obj;
};


////////////////////////// функция для вызова функции ///////////////////////
/// функция для вызова функции func объекта obj через delay секунд, повторять вызов times раз (times = -1 - повторять бесконечно)
template<class T> void RunFunc(T* obj, void (T::*func)(void*), float delay, float times = 1)
{
	obj->AddAction(new CActionRepeate(new CActionSequence(new CActionWaiting(delay), new CActionRunEvent(obj, func), NULL), times), obj);
}

#define ACTION_WAIT_AND_RUN_EVENT(time, node, callbackMethod) new CActionSequence(new CActionWaiting(time),new CActionRunEvent(node, &callbackMethod),NULL)
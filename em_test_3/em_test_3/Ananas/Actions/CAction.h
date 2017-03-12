/*
 *  CActionNew.h
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 11.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include <vector>
#include "Trace.hpp"
#include "CNode.h"
#include "sColor.h"

#define SAFE_DELETE_ACTION(a){ if (a != NULL){a->Delete(); a = NULL;}}

class CActionMngr;

class CActionLink;
////////////////////// action  /////////////////// 
//// базовый класс для всех экшенов

class CAction
{
public:
                        CAction();
                        CAction(const CAction&  action);
	virtual				~CAction();
	virtual void		Update(float dt) = 0;
	
	virtual CAction*	GetCopy() = 0;							// создает копию экшена
	virtual CAction*	GetReversCopy(){ return GetCopy(); }	// создает обратную копию экшена (например если это движение, то обратный экшен будет двигать элемент из конца в начало)

	virtual float		GetDurationTime() = 0;					// взять время выполнения (если <0 выполняется бесконечно, если =0 мгновенно(за 1 такт))
	
	void				Delete(bool runOnDeleteHandler = true);                       		// каждый экшен должен сам себя удалить после окончания работы (вызвать эту функцию)
	bool				IsDelete();
	
    bool                        AddLink(CNode* node);                   // если в конструктор передаем нод,вызывать в конструкторе данный метод
    void                        RemoveLink(CNode* node);                // удаляем связи этого экшна с нодом, если нод удаляется, а экшн к нему не привязан и должен продолжать работать
    const std::list<CNode*>&    GetLinkNodes() { return m_linkNodes; }  // список обьектов CNode, прилинкованых к этому CAction
    void                        CopyLinksFromAction(CAction* action);   // копирование линков из указанного обьекта CAction в текущий CAction
    
    template<class T>                                                   // выставляет обработчика на вызов Delete()
    CAction*			        SetOnDeleteHandler(T* obj, void (T::*handler)(void*))
                                                    {if(!m_deleteEvent) m_deleteEvent = new CEvent(); obj->AddEventHandler(*m_deleteEvent, handler); return this; }
private:
	bool                    m_isDelete; // flag, if true - this Action will be deleted in next Update() call of CActionMngr

    std::list<CActionLink*> m_links;        // links from this Action to CNode objects, which it attached to
    std::list<CNode*>       m_linkNodes;    // list of CNode objects, to which this Action is attached
    
    CEvent*		            m_deleteEvent;  // event which will be called before deleting of this Action
};

// class to handle links between Actions and CNode objects
// basically used to delete complex Actions, when some node, attached to this Action should be deleted
class CActionLink: public CAction
{
    CActionLink(const CActionLink&  action){}
public:
    CActionLink(CAction* link, CNode* linkNode):mLink(link),mLinkNode(linkNode){}
    ~CActionLink()               {if (mLink) mLink->Delete();}
    
    void        Unlink()         {mLink = NULL; Delete();}
    
    CAction*	GetCopy()        { return NULL;}
    float		GetDurationTime(){ return 0;}
    void		Update(float dt) {}
    CNode*      GetLinkNode() { return mLinkNode; } // получить нод, к которому добавлен этот экшн
private:
    CAction*    mLink;      // Action which should be deleted in case we delete this object
    CNode*      mLinkNode;  // node linked to this Action
};



//////////////// CActionMngr /////////////////
/// менеджер управляющий экшенами
/// экземпляр этого класса доступен через CDirector::GetActionManager()
class CActionMngr
{
protected:
    friend class CNode;
	void AddAction(CAction* act, CNode* parent);
	void RemoveAction(CAction* act);
	void RemoveActionForNode(CNode* node);  // set m_isDelete flag to each Action of provided node, Actions will be deleted in next Update() call
    void RemoveActionForNodeImmediatelly(CNode* node);  // don't wait until next Update() call and make delete call for each Action of provided node
public:
	bool IsActionExist(CAction* act);
	bool IsNodeInActions(CNode* node);
	void RemoveAllActions();
	
private:
    typedef std::pair< CAction*, CNode* > ActNod;
	std::list< ActNod >   m_actions;  // list of nodes and Actions attached to them
	
	CActionMngr() {}
	~CActionMngr();
	void Update(float dt);
	
	friend class CDirector;
};



////////////////////// шаблон для модификаторов свойств спрайта ////////////////////////////
template< class T, class C >
class CActionParameterChange: public CAction
{
public:
	typedef C* (C::*hnd_Set)(const T&);
	typedef const T& (C::*hnd_Get)()const;
    
    T GetDest() { return m_dest; }

    static float Abs(vec2 value)   { return value.length(); }
    static float Abs(sColor)       { return 0; }
    static float Abs(float value)  { return fabs(value); }
    
public:
	CActionParameterChange(const CActionParameterChange& act)
    :CAction(act),
    m_object(act.m_object),
	m_Set(act.m_Set),
	m_Get(act.m_Get),
	m_firstPoint(act.m_firstPoint),
	m_dest(act.m_dest),
	m_time(act.m_time),
	m_timeMax(act.m_timeMax),
	m_speed(act.m_speed),
	m_resetFirsPoint(act.m_resetFirsPoint),
	m_resetEndPoint(act.m_resetEndPoint),
    m_step(act.m_step),
    m_stepTimeCount(act.m_stepTimeCount),
    m_stepTime(act.m_stepTime)
    {
        
    }
    
	CActionParameterChange(C* node, hnd_Set Set, T from, T to, float time, float speed = 0, double _step = 0)
    :m_object(node),
    m_Set(Set),
	m_Get(NULL), 
	m_firstPoint(from),
	m_dest(to),
	m_timeMax((speed) ? Abs(to - from)/speed : time),
	m_time(0),
	m_speed(0),
	m_resetFirsPoint(false),
	m_resetEndPoint(false),
    m_step(_step),
    m_stepTimeCount(0.0f)
	{
        m_step = m_step > Abs(m_dest - m_firstPoint) ? Abs(m_dest - m_firstPoint) : m_step;
        m_stepTime = time / (Abs(m_dest - m_firstPoint) * (1 / m_step));
		if (m_timeMax < 0)
        {
            ALOG("warning: action incorrect!");
            Delete();	
        }
        
       AddLink(dynamic_cast< CNode* >(node));
	}

	static CActionParameterChange* To(C* node, hnd_Set Set, hnd_Get Get, T to, float time, float speed = 0)
	{
		CActionParameterChange* newEl = new CActionParameterChange(node, Set, to, to, time, speed);
		newEl->m_Get = Get;
		newEl->m_resetFirsPoint = true;
		if(speed)
		{
			newEl->m_timeMax =-1; 
			newEl->m_speed = speed;
		}
		return newEl;
	}
	
	static CActionParameterChange* By(C* node, hnd_Set Set, hnd_Get Get, T to, float time, float speed = 0)						
	{
		CActionParameterChange* newEl = new CActionParameterChange(node, Set, to, to, time, speed);
		newEl->m_Get = Get;
		newEl->m_resetFirsPoint = true;
		newEl->m_resetEndPoint = true;
		if(speed) newEl->m_timeMax = Abs(to) / speed;
		return newEl;
	}
	
	void Update(float dt)																		
	{
        if(IsDelete())
            return;
		if (m_resetFirsPoint)
		{
			m_firstPoint = (((C*)m_object)->*m_Get)(); 		
			if (m_resetEndPoint) m_dest += (((C*)m_object)->*m_Get)(); 
			m_resetEndPoint = m_resetFirsPoint = false;
			if (m_timeMax == -1)
			{
				if (m_speed) m_timeMax = Abs(m_dest - m_firstPoint) / m_speed;
				else Delete();
			}
		}
		m_time += dt;
		float pathProc = m_time / m_timeMax;
		
		if (pathProc >= 1.0f)
		{
			(((C*)m_object)->*m_Set)(m_dest);
			Delete();
            
			return;
		}
        
        if(m_step)
        {
            if(m_time > m_stepTimeCount)
            {
                m_stepTimeCount += m_stepTime;
                (((C*)m_object)->*m_Set)(m_firstPoint + (m_dest - m_firstPoint) * pathProc);
            }
        }else
        {
            (((C*)m_object)->*m_Set)(m_firstPoint + (m_dest - m_firstPoint) * pathProc);
        }
	}
	
	CAction*						GetCopy()
    {
        return new CActionParameterChange(*this);
    }
    
	CAction*						GetReversCopy()		        
	{
		CActionParameterChange* res = (CActionParameterChange*)GetCopy();
		if (m_dest == m_firstPoint) 
		{
			if (m_resetEndPoint)
				res->m_dest = -m_dest;
		}
		else 
		{
			res->m_firstPoint = m_dest;
			res->m_dest = m_firstPoint;
		}
		return res;
	}	
	float							GetDurationTime()	
    {
        return m_timeMax;
    }
	
protected:
    C*      m_object;   // object, which variable value will be changed, usually is heir of CNode
	hnd_Set	m_Set;      // pointer to variable setter in object's class
	hnd_Get	m_Get;      // pointer to variable getter in object's class
	T		m_firstPoint;   // variable value to start with
	T		m_dest;         // the final value of a variable
	float	m_time;         // Action timer
	float	m_timeMax;      // finish time of Action
	float	m_speed;        // if not 0, timeMax will be equal to Abs(m_dest - m_firstPoint) / m_speed;
	bool	m_resetFirsPoint;   // flag to change m_firstPoint value during Update() call
	bool	m_resetEndPoint;    // flag to change m_dest value during Update() call
    double   m_step;            // if true - variable will be changed only after time setted m_stepTime is end
    float    m_stepTimeCount;   // additional timer for step changing
    float    m_stepTime;        // time after variable should be changed in case m_step is true
};



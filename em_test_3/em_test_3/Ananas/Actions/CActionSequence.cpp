/*
 *  CActionSequence.cpp
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 12.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CActionSequence.h"
#include "../CDirector.h"

CActionSequence::CActionSequence(bool autodelete)
: m_curAction(0),
  m_autoDelete(autodelete),
  m_inUpdate(false)
{
}

CActionSequence::CActionSequence(CAction* first, CAction* next, ...)
: m_curAction(0),
m_autoDelete(true),
m_inUpdate(false)
{
    if (!first) { Delete(); return; }
    m_actions.push_back( first );
    if (next)
    {
        m_actions.push_back( next );
        va_list uk_arg;
        va_start( uk_arg, next );
        CAction* p;
        while ( ( p = va_arg( uk_arg, CAction* ) ) )
        {
            m_actions.push_back( p );
            
        }
        va_end( uk_arg );
    }
	
    CheckForLinks();
}

CActionSequence::CActionSequence(const std::vector< CAction* >& els)
: m_curAction(0),
  m_actions(els),
m_autoDelete(true),
m_inUpdate(false)
{
    if (els.size() == 0) 
        Delete();
    CheckForLinks();
}

CActionSequence::CActionSequence(const CActionSequence& action):
CAction(action),
m_curAction(0),
m_autoDelete(action.m_autoDelete),
m_inUpdate(false)
{
    m_actions.resize(action.m_actions.size());
    for (int i(0); i<m_actions.size(); i++)
    {
        m_actions[i] = action.m_actions[i]->GetCopy();
    }
    CheckForLinks();
}

CActionSequence::~CActionSequence()
{
	std::vector< CAction* >::iterator i = m_actions.begin();
	for (; i != m_actions.end(); i++)
	{
		delete *i;
        (*i) = NULL;
	}
}

void CActionSequence::CheckForLinks()
{
    std::vector< CAction* >::iterator i = m_actions.begin();
	for (; i != m_actions.end(); i++)
	{
		CAction* act = *i;
        CopyLinksFromAction(act);
	}
}

void CActionSequence::Update(float dt)
{
    if(IsDelete())
        return;
    if(m_actions.size()==0)
        return;
    m_inUpdate = true;
    if (m_autoDelete)
    {
        CAction *act = m_actions[m_curAction];
        act->Update(dt);
        if (act->IsDelete())
        {
            RemoveAction(act);
            m_curAction++;
            if (m_curAction >= m_actions.size()) Delete();
        }
    }
    else
    {
        if (m_actions.size() > 0)
        {
            CAction *act = m_actions.front();
            act->Update(dt);
            if (act->IsDelete())
            {
                vector<CAction*>::iterator erase = std::find(m_actions.begin(), m_actions.end(), act);
                if (erase != m_actions.end())
                {
                    RemoveAction(act);
                    m_actions.erase(erase);
                    delete act;
                }
            }
        }
    }
    
    for (vector<CAction*>::iterator it = m_delete_buffer.begin(); it != m_delete_buffer.end(); it++)
    {
        CAction *action = *it;
        RemoveAction(action);
        delete action;
        action = NULL;
    }
    m_delete_buffer.clear();
    
    m_inUpdate = false;
}

void CActionSequence::RemoveAction(CAction *action)
{
    const std::list<CNode*>& linkNodes = action->GetLinkNodes();
    for (std::list< CNode* >::const_iterator it = linkNodes.begin(); it != linkNodes.end(); it++)
        if (*it) RemoveLink((*it));
}

CAction* CActionSequence::GetCopy()
{
	CActionSequence* res = new CActionSequence(*this);
	return res;
}

CAction* CActionSequence::GetReversCopy()
{
	CActionSequence* res = new CActionSequence(*this);
	for (int i = 0; i < res->m_actions.size(); i++)
	{
        delete res->m_actions[i];
		res->m_actions[i] = m_actions[res->m_actions.size()-i-1]->GetReversCopy();
	}
    res->CheckForLinks();
	return res;
}

CAction* CActionSequence::Add(CAction* act, int position)
{
    if (position == -1)
        m_actions.push_back( act );
    else
        m_actions.insert(m_actions.begin()+position, act);
    
    CopyLinksFromAction(act);
    
    return this;
}

void CActionSequence::Clear()
{
    DEBUG_ASSERT(!m_autoDelete); // https://jira.pacific-interactive.net/browse/HOFM-17075
    
    if (m_inUpdate)
    {
        m_delete_buffer = m_actions;
        m_actions.clear();
        return;
    }
    
    for (vector<CAction*>::iterator it = m_actions.begin(); it != m_actions.end(); it++)
    {
        CAction *action = *it;
        RemoveAction(action);
        delete action;
        (*it) = NULL;
    }
    m_actions.clear();
}

float CActionSequence::GetDurationTime()
{
	float sum = 0;
	for (int i = 0; i < m_actions.size(); i++)
	{
		float time = m_actions[i]->GetDurationTime();
		if (time < 0) return -1;
		sum += time;
	}
	
	return sum;
}

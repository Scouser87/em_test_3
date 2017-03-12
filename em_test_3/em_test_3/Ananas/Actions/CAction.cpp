/*
 *  CActionNew.cpp
 *  Wooliz
 *
 *  Created by Anton Zagovalko on 11.07.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CAction.h"
#include "CDirector.h"

CActionMngr::~CActionMngr()
{
	std::list< ActNod >::reverse_iterator i(m_actions.rbegin());
	for (; i != m_actions.rend(); ++i)
	{
		delete i->first;
	}
    
    m_actions.clear();
}

void CActionMngr::AddAction(CAction* act, CNode* parent)
{
	m_actions.push_back(ActNod(act, parent));
}

void CActionMngr::RemoveAction(CAction* act)
{
	std::list< ActNod >::iterator i(m_actions.begin());
	for (; i != m_actions.end(); i++) 
	{
		if (i->first == act) 
		{
			act->Delete();		
			return;
		}
	} 
}

void CActionMngr::RemoveActionForNode(CNode* node)
{
	std::list< ActNod >::iterator i(m_actions.begin());
	for (; i != m_actions.end(); i++) 
	{
		if (i->second == node) 
		{
			i->first->Delete();
		}
	} 	
}

void CActionMngr::RemoveActionForNodeImmediatelly(CNode* node)
{
    for (std::list< ActNod >::iterator i(m_actions.begin()); i != m_actions.end(); )
	{
        std::list<ActNod>::iterator it = i;
        i++;
		if (it->second == node)
		{
			delete it->first;
			m_actions.erase(it);
		}
	}
}

bool CActionMngr::IsActionExist(CAction* act)
{
	for (std::list< ActNod >::iterator i(m_actions.begin()); i != m_actions.end(); i++)
	{
		if (i->first == act) return true;
	}
	
	return false;
}

bool CActionMngr::IsNodeInActions(CNode* node)
{
	for (std::list< ActNod >::iterator i(m_actions.begin()); i != m_actions.end(); i++)
	{
		if (i->second == node)
			return true;
	}
	
	return false;
}

void CActionMngr::RemoveAllActions()
{
    for (std::list< ActNod >::iterator i(m_actions.begin()); i != m_actions.end();)
	{
        delete i->first;
	}
    m_actions.clear();
}

void CActionMngr::Update(float dt)
{
	for (std::list< ActNod >::iterator i(m_actions.begin()); i != m_actions.end(); i++)
	{
        ActNod nod = *i;
		if ( !nod.first->IsDelete() && (!nod.second->IsPause() || nod.second->IsForcedActionUpdate()) )
            nod.first->Update(dt);
	}
    for (std::list< ActNod >::iterator i(m_actions.begin()); i != m_actions.end(); )
	{
        std::list<ActNod>::iterator it = i;
        i++;
		if (it->first->IsDelete())
		{
            CNode* thisNode= it->second;
			delete it->first;
			m_actions.erase(it);
            thisNode->SetHasActions(IsNodeInActions(thisNode));
		}
	}
}


CAction::CAction()
:m_isDelete(false)
,m_deleteEvent(NULL)
{
    
}

CAction::CAction(const CAction&  action)
:m_isDelete(false)
,m_deleteEvent(action.m_deleteEvent ? new CEvent(*action.m_deleteEvent) : NULL) //почему? в CNode тоже копирует m_deleteEvent
{
    for (std::list< CNode* >::const_iterator it = action.m_linkNodes.begin(); it != action.m_linkNodes.end(); it++)
        AddLink((*it));
}

CAction::~CAction()
{
    if (m_deleteEvent)
        delete m_deleteEvent;
    
    for (std::list<CActionLink*>::iterator it = m_links.begin(); it!=m_links.end(); it++)
        (*it)->Unlink();
}


bool CAction::AddLink(CNode* node)
{
    if (!node)
        return false;
    
    for (std::list<CNode*>::iterator it = m_linkNodes.begin(); it != m_linkNodes.end(); it++)
    {
        if (*it == node)
            return true;
    }
    m_linkNodes.push_back(node);
    CActionLink* link = new CActionLink(this, node);
    node->AddAction(link);
    m_links.push_back(link);
    
    return true;
}

void CAction::RemoveLink(CNode* node)
{
    if (!node)
        return;
    
    for (std::list<CActionLink*>::iterator it = m_links.begin(); it!=m_links.end(); it++)
    {
        if ((*it)->GetLinkNode() == node)
        {
            (*it)->Unlink();
            m_links.erase(it);
			break;
        }
    }
}

void CAction::CopyLinksFromAction(CAction* action)
{
    const std::list<CNode*>& linkNodes = action->GetLinkNodes();
    for (std::list< CNode* >::const_iterator it = linkNodes.begin(); it != linkNodes.end(); it++)
        AddLink((*it));
}

void CAction::Delete(bool runOnDeleteHandler)
{
    
    if(m_deleteEvent)
    {
        CEvent*	event(m_deleteEvent);//чтобы при повторном вызове не возникала рекурсия
        m_deleteEvent = NULL;
        if (runOnDeleteHandler)
            event->RunEvent(this);
        delete event;
    }

    for (std::list<CActionLink*>::iterator it = m_links.begin(); it!=m_links.end(); it++)
        (*it)->Unlink();
    m_links.clear();
    
    m_isDelete = true;
}
bool CAction::IsDelete()
{
    return m_isDelete;
}
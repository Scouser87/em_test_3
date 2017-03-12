/*
 *  CNode.cpp
 *  Ananas
 *
 *  Created by Anton Zagovalko on 24.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CNode.h"
#include "Trace.hpp"
#include "Actions/CAction.h"
#include "CDirector.h"

CNode::CNode()
:m_parent(		NULL),
m_isPause(		false),
m_isParentPause(false),
m_deleteEvent(CEvent()),
m_singleTouch(  -1),
m_isTouchOn(    true),
m_hasActions(   false),
m_userData(NULL),
m_forceActionsUpdate(false)
{
    CheckParentsPause();
    if (s_shouldCountNodes)
        ++s_nodeCounter;
}

CNode::CNode(const CNode& node)
: m_parent(     NULL),
m_isPause(      node.m_isPause),
m_isParentPause(false),
m_deleteEvent(  node.m_deleteEvent),
m_singleTouch(  node.m_singleTouch),
m_isTouchOn(    node.m_isTouchOn),
m_hasActions(   node.m_hasActions),
m_name(         node.m_name),
m_forceActionsUpdate(node.m_forceActionsUpdate)
{
    std::list< CNode* >::const_iterator i = node.m_children.begin();
    for (; i != node.m_children.end(); i++)
    {
        AddChild((*i)->GetCopy());
    }
    CheckParentsPause();
    if (s_shouldCountNodes)
        ++s_nodeCounter;
}

CNode::~CNode()
{
    if(!m_isDelete)
        ALOG("Error!!!! Incorrect delete node!!!! Name:\"%s\"", m_name.c_str());
    if (m_children.size() > 0)
    {
        ALOG("Error this node \"%s\" didn't delete his children!!!", m_name.c_str());
        Delete();
        CheckDelete();
        m_children.clear();
    }
    if (s_shouldCountNodes)
    {
        if (s_nodeCounter > 0)
            --s_nodeCounter;
    }
}

CNode* CNode::AddChild(CNode* node)
{
    if (node == this) return NULL;
    if(node == NULL) { ALOG("error: node is NULL!"); return NULL;}
    for(CNode* parent = GetParent(); parent != NULL; parent = parent->GetParent())
        if(node == parent) return NULL;
	if (node->m_parent)
	{
        if (node->m_parent == this) return NULL;
		node->m_parent->RemoveChild(node, false);
	}
	
    m_children.push_back(node);
    
    
	node->m_parent = this;
	
  //  node->m_isPause = m_isPause;
    node->CheckParentsPause();
	// HUD
	/*if (GetIsHUD())
	{
		CSprite* ch = dynamic_cast< CSprite* > (node);
		if (ch) ch->SetIsHUD(true);
	}*/
	
	return this;
}

CNode* CNode::RemoveChild(CNode* node, bool isDelete)
{
    
    std::list< CNode* >::iterator i = m_children.begin();
    for (; i != m_children.end(); i++)
    {
        if (*i == node)
        {
            if (isDelete)
            {
                node->Delete();
            }
            else
            {
                m_children.erase(i);
				node->m_parent = NULL;
				break;
            }
        }//end if (*i == node)
    }//end for

    return this;
}

CNode* CNode::RemoveAllChild()
{
    if ( !m_children.empty() )
    {
        std::list< CNode* >::iterator i = m_children.begin();
        while (i != m_children.end())
        {
            (*i++)->Delete();
        }
    }
	return this;
}

void CNode::Swap(CNode* child1, CNode* child2)
{
    if (child1 == child2)
    {
        ALOG("Node Swap error : children are similar!");
        return;
    }    
    if (!child1->GetParent())
    {
        ALOG("Node Swap error : child1 has no parent!");
        return;
    }
    if (!child2->GetParent())
    {
        ALOG("Node Swap error : child2 has no parent!");
        return;
    }
    
    CNode* parent = child1->GetParent();
    if (parent != child2->GetParent())
    {
        ALOG("Node Swap error : children have different parents!");
        return;
    }
    
    int z1 = child1->GetZOrder();
    int z2 = child2->GetZOrder();
    child1->SetZOrder(z2);
    child2->SetZOrder(z1);
}

void CNode::Simulate(float dt)
{
	if (IsPause()) return;

    if (!m_isDelete)
    {
        if (s_shouldCountNodes)
            s_nodeUpdatedCounter++;
        Update(dt);
    }
    
    if ( !m_children.empty() )
    {
        std::list< CNode* >::iterator i = m_children.begin();
        for (; i != m_children.end(); i++)
        {
            (*i)->Simulate(dt);
        }
    }
}

void CNode::CheckDelete()
{
    if ( !m_children.empty() && m_children.size() > 0 )
    {
        for (std::list< CNode* >::iterator i = m_children.begin(); i != m_children.end();)
        {
            std::list<CNode*>::iterator it = i;
            i++;
            CNode* node = *it;
            if (node)
            {
                node->CheckDelete();
                if (node->IsDelete())
                {
#ifdef CHECK_DELETE_IMPROVEMENT
                    CNode::DeleteFromMemory(node);
#else
                    node->RemoveAllEventListen();
                    node->ClearActions();
                    delete node;
#endif
                    m_children.erase(it);
                }
            }
            else
            {
                ALOG("Error!!! Null child in node %s !", GetName().c_str());
            }
        }
    }
}

#ifdef CHECK_DELETE_IMPROVEMENT
void CNode::DeleteFromMemory(CNode* node)
{
    node->RemoveAllEventListen();
    node->ClearActions();
    delete node;
}
#endif  // CHECK_DELETE_IMPROVEMENT

void CNode::Visit()
{
    if (m_isDelete) return;
    
    if ( !m_children.empty() )
    {
        std::list< CNode* >::iterator i = m_children.begin();
        for (; i != m_children.end(); i++)
        {
            (*i)->Visit();
        }
    }
}

CNode* CNode::SetPause(bool pause)
{
    if (m_isPause == pause)
        return this;
    
	m_isPause = pause;
    
    bool isPause = false;
    CNode* parent = GetParent();
    while(parent!=NULL)
    {
        if(parent->IsPause())
        {
            isPause = true;
            break;
        }
        parent = parent->GetParent();
    }
    m_isParentPause = isPause;
    
    std::list< CNode* >::iterator i = m_children.begin();
    for (; i != m_children.end(); i++)
    {
        (*i)->CheckParentsPause();
    }
	return this;
}

void CNode::CheckParentsPause()
{
    bool isPause = false;
    CNode* parent = GetParent();
    while(parent!=NULL)
    {
        if(parent->IsPause())
        {
            isPause = true;
            break;
        }
        parent = parent->GetParent();
    }
    m_isParentPause = isPause;
    std::list< CNode* >::iterator i = m_children.begin();
    for (; i != m_children.end(); i++)
    {
        (*i)->CheckParentsPause();
    }
}


bool CNode::IsPause(bool askParent)
{
    if (askParent)
        return (m_isPause || m_isParentPause);
    else
        return m_isPause;
}

CNode* CNode::SetIsSingleTouch(bool isSingle)
{
    m_singleTouch = -1 + (int)isSingle;
    return this;
}

CNode* CNode::SetIsTouchesOnRecursive(bool isTouch)
{
     m_isTouchOn = isTouch;
     std::list< CNode* >::iterator it = GetChildren().begin();
     for (; it != GetChildren().end(); it++)
     {
        (*it)->SetIsTouchesOnRecursive(isTouch);
     }
     return this;
}

CNode* CNode::SetIsTouchesOn(bool isTouch)
{
    m_isTouchOn = isTouch;
    return this;
}

CNode* CNode::SetUserData(void* userData)
{
    m_userData = userData;
    return this;
}

bool CNode::GetIsSingleTouch()const
{
    return (m_singleTouch != -1);
}

bool CNode::GetIsTouchesOn()const
{
    return m_isTouchOn;
}

bool CNode::TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed)
{
	return isPrecessed;
}

bool CNode::TouchMove(intptr_t ID, vec2& pos, bool isPrecessed)
{
	return isPrecessed;
}

bool CNode::TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed)
{
	return isPrecessed;
}

void CNode::TouchCancel(intptr_t ID, vec2& pos)
{
}

void CNode::TouchReset(intptr_t ID, vec2& pos)
{
}

void CNode::TouchResetSys(intptr_t ID, vec2& pos)
{
    if (m_singleTouch != -1)
        m_singleTouch = 0;
    
    TouchReset(ID, pos);
    
    std::list< CNode* >::reverse_iterator i = m_children.rbegin();
    for (; i != m_children.rend(); i++)
    {
        CNode* node=(*i);
        node->TouchResetSys(ID, pos);
    }
}

bool CNode::TouchProcess(ETochType type, intptr_t ID, vec2& pos, bool isPrecessed)
{
	if (IsPause() || !m_isTouchOn)
    {
        if (type == ettEnd || type == ettCancel)
            TouchResetSys(ID, pos);
        return false;
    }
    
    if (m_singleTouch != -1)
    {
        if (((type == ettMove || type == ettEnd) && m_singleTouch != ID) ||
            (type == ettBegin && m_singleTouch != 0))
            return false;
        
        if (type == ettBegin) 
            m_singleTouch = ID;
        
        if (type == ettEnd || type == ettCancel)
            m_singleTouch = 0;
    }
    
    std::vector<CNode *> children;
    children.reserve(m_children.size());
    std::list< CNode* >::reverse_iterator rIt = m_children.rbegin();
    while (rIt != m_children.rend())
    {
        children.push_back(*rIt);
        ++rIt;
    }
    for (int i(0); i < children.size(); i++)
    {
        CNode* node = children[i];
        if (node->TouchProcess(type, ID, pos, isPrecessed))
            isPrecessed = true;
    }
    
    if (type == ettBegin)
	{
		if (TouchBegin(ID, pos, isPrecessed)) 
			isPrecessed = true;
	}
	else if (type == ettMove)
	{
		if (TouchMove(ID, pos, isPrecessed)) 
			isPrecessed = true;
	}
	else if (type == ettEnd)
	{
        if (TouchEnd(ID, pos, isPrecessed))
            isPrecessed = true;
	}
    else if (type == ettCancel)
    {
        TouchCancel(ID,pos);
        isPrecessed = false;
    }
    
	return isPrecessed;
}

CNode* CNode::GetParent() const
{
	return m_parent;
}

std::list< CNode* >& CNode::GetChildren()
{
	return m_children;
}

CNode* CNode::GetChildByZ(int z)
{
    int size(0);
    size = m_children.size();
    if (z < 0 || z >= size) return NULL;
    
    std::list< CNode* >::iterator i = m_children.begin();
    for (; i != m_children.end(); i++, z--)
    {
        if (z == 0) return *i;
    }
    return NULL;
}

CNode* CNode::GetChildByZ(const std::list<int>& zIndex)
{
    CNode* child(NULL);
    if (zIndex.size())
    {
        child = this;
        for (std::list<int>::const_iterator it(zIndex.begin()); it!=zIndex.end(); it++)
        {
            child = child->GetChildByZ(*it);
            if (child == NULL)
                return NULL;
        }
    }
    
    return child;
}

CNode* CNode::GetFirstChild()
{
    if (m_children.size() > 0) 
        return m_children.front();
    return NULL;    
}

CNode* CNode::GetPrevSibling()
{
    CNode* parent = GetParent();
    std::list<CNode*>::reverse_iterator r_it = parent->m_children.rbegin();
    while (r_it != parent->m_children.rend())
    {
        if ((*r_it)==this)
        {
            r_it++;
            if (r_it != parent->m_children.rend())
                return (*r_it);
            else
                return NULL;
        }
        r_it++;
    }
    return NULL;
}

CNode* CNode::GetNextSibling()
{
    CNode* parent = GetParent();
    std::list<CNode*>::iterator it = parent->m_children.begin();
    while (it != parent->m_children.end())
    {
        if ((*it)==this)
        {
            it++;
            if (it != parent->m_children.end()) 
                return (*it);
            else
                return NULL;
        }
        it++;
    }
    return NULL;
}

bool CNode::IsNodeIncluded(CNode* node)const
{
	bool isInclude = false;
    
    std::list< CNode* >::const_iterator i = m_children.begin();
    for (; i != m_children.end(); i++)
    {
        if (node == *i || (*i)->IsNodeIncluded(node))
        {
            isInclude = true;
            break;
        }
    }
	return isInclude;
}

CEvent& CNode::GetEventOnDelete()
{
    return m_deleteEvent;
}

#ifdef CHECK_DELETE_IMPROVEMENT

void CNode::Delete()
{
    if (!m_isDelete)
    {
        CDirector::GetDirector().DeleteNode(this);
        DeleteWithChildren();
    }
}

void CNode::DeleteWithChildren()
{
    if (!m_isDelete)
    {
        m_isDelete = true;
        m_isParentPause = false;
        m_isPause = false;
        
        m_deleteEvent.RunEvent(this);
    }
    std::list< CNode* >::iterator i = m_children.begin();
    for (; i != m_children.end(); i++)
    {
        (*i)->DeleteWithChildren();
    }
}

#else   // CHECK_DELETE_IMPROVEMENT

void CNode::Delete()
{
    DEBUG_ASSERT(GetParent());
    
    m_isDelete = true;
    m_isParentPause = false;
    m_isPause = false;
    
    m_deleteEvent.RunEvent(this);
    
    std::list< CNode* >::iterator i = m_children.begin();
    for (; i != m_children.end(); i++)
    {
        (*i)->Delete();
    }
}

#endif  // CHECK_DELETE_IMPROVEMENT

CNode* CNode::AddChild(CNode* node, int z)
{    
	AddChild(node);
	node->SetZOrder(z);    
	return this;
}

CNode* CNode::SetZOrder(int z)
{
	if (!m_parent) return this;
	if (z < 0) z = 0;
    
    int size(0);
    size = m_parent->m_children.size();

	if (z >= size) z = size - 1;
    
	std::list< CNode* >::iterator i = m_parent->m_children.begin();
	for (; i != m_parent->m_children.end(); i++)
	{
		if (*i == this)
		{
			m_parent->m_children.erase(i);
			break;
		}
	}
    
    if (z == m_parent->m_children.size())
        m_parent->m_children.push_back(this);
    else
    {
        std::list< CNode* >::iterator i = m_parent->m_children.begin();
        for (int a = 0; i != m_parent->m_children.end(); i++, a++)
        {
            if (a == z)
            {
                m_parent->m_children.insert(i, this);
                break;
            }
        }
    }

	return this;
}

int CNode::GetZOrder() const
{
    if ( !m_parent ) return 0;
	int a = 0;
    std::list< CNode* >::iterator i = m_parent->m_children.begin();
    for (; i != m_parent->m_children.end(); i++, a++)
        if (*i == this)
            return a;
	
	
	return 0;
}

void CNode::SetName(const std::string& name)
{
	m_name = name;
}

const std::string& CNode::GetName() const
{
	return m_name;
}

void* CNode::GetUserData()
{
    return m_userData;
}

bool CNode::GetIsHUD() const
{
	return false;
}

bool CNode::GetIsHUDThisOrParent()
{
	if (GetIsHUD()) return true;
    if (GetParent()) 
        return GetParent()->GetIsHUDThisOrParent();
    return false;
}

CNode* CNode::GetCopy()
{
	return new CNode(*this);
}

std::list<CNode*> CNode::GetParentTree(const CNode* progenitor)
{
    std::list<CNode*> listParents;
    CNode* parent(this->GetParent());
    listParents.push_front(parent);
    //цикл конечный, поскольку root->GetParent() == NULL
    // проверка на parent==NULL, на случай если progenitor не является прародителем this
    while (parent && parent!=progenitor)
    {
        parent = parent->GetParent();
        listParents.push_front(parent);
    }
    // проверка на случай если progenitor не является прародителем this
    if (*listParents.begin() != progenitor)
    {
        ALOG("Error GetParentTree(progenitor): progenitor is not an ancestor");
        listParents.clear();
    }
    
    return listParents;
}

std::list<int> CNode::GetIndexChild(const CNode* progenitor)
{
    std::list<int> indexChild;
    CNode* child(this);
    indexChild.push_front(child->GetZOrder());
    CNode* parent(child->GetParent());
    //цикл конечный, поскольку root->GetParent() == NULL
    // проверка на parent==NULL, на случай если progenitor не является прародителем this
    while (parent && parent!=progenitor)
    {
        child = parent;
        indexChild.push_front(child->GetZOrder());
        parent = parent->GetParent();
    }
    // проверка на случай если progenitor не является прародителем this
    if (parent != progenitor)
    {
        ALOG("Error GetIndexParentTree(progenitor): progenitor is not an ancestor");
        indexChild.clear();
    }
    
    return indexChild;
}


CNode* CNode::AddAction(CAction* action)
{
	CDirector::GetActionManager().AddAction(action, this);
    m_hasActions = true;
    return this;
}

void CNode::ClearActions()
{
    if(m_hasActions)
    {
        m_hasActions=false;
        CDirector::GetActionManager().RemoveActionForNodeImmediatelly(this);
    }
}


CNode* CNode::GetSubNodeByName(std::string name)
{
    CNode* result(NULL);
    
    NODE_LOOP(this, it)
    {
        CNode* node = *it;
        if (!node->GetName().compare(name))
        {
            result = node;
            break;
        }
        result = node->GetSubNodeByName(name);
        if (result)
            break;
    }
    
    return result;
}

//////////////////////////////// Nodes count logic ////////////////////////////////

unsigned long long  CNode::s_nodeCounter = 0;
unsigned long long  CNode::s_nodeUpdatedCounter = 0;
bool                CNode::s_shouldCountNodes = false;

const unsigned long long& CNode::GetNodeCount()
{
    return s_nodeCounter;
}

void CNode::ResetNodeUpdatedCount()
{
    s_nodeUpdatedCounter = 0;
}

const unsigned long long& CNode::GetNodeUpdatedCount()
{
    return s_nodeUpdatedCounter;
}

void CNode::SetCountNodes(bool value)
{
    s_shouldCountNodes = value;
}

//////////////////////////////// Nodes count logic end ////////////////////////////////

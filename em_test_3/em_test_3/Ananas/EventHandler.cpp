//
//  EventHandler.cpp
//  House_of_Fun_2_37
//
//  Created by Andrew Godny on 15.02.17.
//  Copyright © 2017 PacificInteractive. All rights reserved.
//

#include "EventHandler.h"
#include <algorithm>


#pragma mark EventHandler

EventHandler::EventHandler():
m_isDelete(false)
{
}

EventHandler::EventHandler(const EventHandler& ev):
m_isDelete(false)
{
}

void EventHandler::CopyEventHandler(unsigned long eventFrom, CEvent* eventTo)
{
    std::list< std::pair<unsigned long, hnd> >::iterator it = m_handlers.begin();
    while(it!=m_handlers.end())
    {
        if ((*it).first == eventFrom)
        {
            eventTo->AddEventListener(this);
            m_handlers.push_back(std::pair<unsigned long, hnd>(eventTo->GetID(), (hnd)(*it).second));
        }
        it++;
    }
    
    for (auto it = m_lambdas.begin(); it != m_lambdas.end(); it++)
        if (it->first == eventFrom)
        {
            eventTo->AddEventListener(this);
            m_lambdas.push_back(lambda_pair(eventTo->GetID(), it->second));
        }
}

void EventHandler::RemoveEventListen(CEvent* ev, bool removeListener)
{
    std::list< std::pair<unsigned long, hnd> >::iterator it = m_handlers.begin();
    while (it != m_handlers.end())
    {
        if ((*it).first == ev->GetID())
        {
            it = m_handlers.erase(it);
            continue;
        }
        it++;
    }
    
    for (auto it = m_lambdas.begin(); it != m_lambdas.end(); )
        if (it->first == ev->GetID())
            it = m_lambdas.erase(it);
        else
            it++;
    
    if(removeListener)
        ev->RemoveListener(this);
}

void EventHandler::EventProcess(CEvent* ev, void* params)
{
    std::list<hnd> eventsToExecute;
    for (std::list< std::pair<unsigned long, hnd> >::const_iterator i = m_handlers.begin(); i != m_handlers.end(); i++)
    {
        if ((*i).first == ev->GetID())
            eventsToExecute.push_front((*i).second);
    }
    
    for (std::list<hnd>::const_iterator i = eventsToExecute.begin(); i != eventsToExecute.end(); i++)
        (this->*(*i))(params);
    
    std::for_each(m_lambdas.begin(), m_lambdas.end(), [ev, params](const lambda_pair& pair) {
        if (pair.first == ev->GetID())
            pair.second(params);
    });
}

bool EventHandler::IsDelete()
{
    return m_isDelete;
}

void EventHandler::RemoveAllEventListen()
{
    std::list< std::pair<unsigned long, hnd> >::iterator it = m_handlers.begin();
    while(it!=m_handlers.end())
    {
        ((CEvent*)(*it).first)->RemoveListener(this);
        it = m_handlers.erase(it);
    }
    m_handlers.clear();
    
    for (auto it = m_lambdas.begin(); it != m_lambdas.end(); it++)
        ((CEvent*)(it->first))->RemoveListener(this);
    m_lambdas.clear();
}

CEvent* EventHandler::GetEventWithId(tEventIdType eventId)
{
    std::map<tEventIdType, CEvent>::iterator search = m_events.find(eventId);
    if (search != m_events.end()) {
        return &search->second;
    } else {
        return NULL;
    }
}

void EventHandler::RemoveEventById(tEventIdType eventId)
{
    std::map<tEventIdType, CEvent>::iterator search = m_events.find(eventId);
    if (search != m_events.end())
        m_events.erase(search);
}

EventHandler* EventHandler::RemoveHandler(EventHandler* obj)
{
    std::vector<tEventIdType> eventsToRemove;
    
    for (std::map<tEventIdType, CEvent>::iterator iterator = m_events.begin(); iterator != m_events.end(); ++iterator)
    {
        CEvent& event = iterator->second;
        event.RemoveListener(obj);
        
        if (event.GetListenersCount() <= 0)
            eventsToRemove.push_back(iterator->first);
    }
    
    // if we have no more observers - remove these notifications completely
    for (std::vector<tEventIdType>::iterator iterator = eventsToRemove.begin(); iterator != eventsToRemove.end(); ++iterator)
        RemoveHandlerForEvent(*iterator, obj);
    
    return this;
}

EventHandler* EventHandler::RemoveHandlerForEvent(tEventIdType eventId, EventHandler* obj)
{
    std::map<tEventIdType, CEvent>::iterator search = m_events.find(eventId);
    if (search != m_events.end()) {
        CEvent& event = search->second;
        event.RemoveListener(obj);
        if (event.GetListenersCount() <= 0)
            m_events.erase(search);
    }
    return this;
}

void EventHandler::AddEventHandler(CEvent &ev, EventHandler* lambda_owner, lambda_func handler)
{
    for (auto it = m_lambdas.begin(); it != m_lambdas.end(); it++)
    {
        auto first = it->first;
        auto second = it->second;
        if (first == ev.GetID() && &second == &handler)
            return;
    }
    ev.AddEventListener(this);
    m_lambdas.push_back(lambda_pair(ev.GetID(), handler));
}


#pragma mark CEvent

CEvent::CEvent()
{
    m_id = (unsigned long)this;
    m_attachedNodes.clear();
}

CEvent::CEvent(const CEvent& event)
{
    m_id = (unsigned long)this;
    m_attachedNodes.clear();
    for (int i = 0; i < event.m_attachedNodes.size(); i++)
        event.m_attachedNodes[i]->CopyEventHandler(event.m_id, this);
}

CEvent::~CEvent()
{
    RemoveAllListeners();
}

CEvent& CEvent::operator=(const CEvent& event)
{
    //проверка на самоприсваивание
    if (this == &event) {
        return *this;
    }
    m_id = (unsigned long)this;
    m_attachedNodes.clear();
    for (int i = 0; i < event.m_attachedNodes.size(); i++)
        event.m_attachedNodes[i]->CopyEventHandler(event.m_id, this);
    return *this;
}

void CEvent::AddEventListener(EventHandler* listener)
{
    for (int i = 0; i < m_attachedNodes.size(); i++)
    {
        if (m_attachedNodes[i] == listener)
            return;
    }
    m_attachedNodes.push_back(listener);
}

void CEvent::RemoveListener(EventHandler* listener)
{
    for (int i = 0; i < m_attachedNodes.size(); i++)
    {
        if (m_attachedNodes[i] == listener)
        {
            m_attachedNodes.erase(m_attachedNodes.begin() + i);
            break;
        }
    }
}

void CEvent::RemoveAllListeners()
{
    for (int i = 0; i < m_attachedNodes.size(); i++)
        m_attachedNodes[i]->RemoveEventListen(this, false);
    m_attachedNodes.clear();
}

void CEvent::RunEvent(void* data)
{
    for (int i = 0; i < m_attachedNodes.size(); i++)
    {
        if (!m_attachedNodes[i]->IsDelete())
            m_attachedNodes[i]->EventProcess(this, data);
    }
}

void CEvent::RunAndRemoveAllListeners(void* data)
{
    RunEvent(data);
    RemoveAllListeners();
}

void CEvent::operator()(void* data)
{
    RunEvent(data);
}
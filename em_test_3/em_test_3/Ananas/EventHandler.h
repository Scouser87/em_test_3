//
//  EventHandler.hpp
//  House_of_Fun_2_37
//
//  Created by Andrew Godny on 15.02.17.
//  Copyright © 2017 PacificInteractive. All rights reserved.
//

#pragma once

#include <list>
#include <vector>
#include <map>
#include <functional>
#include "algebra3.h"

typedef int tEventIdType;

#pragma mark General stuff

/******************************************
 Event IDs
 ******************************************/
// Common events
#define EVENT_INIT_COMPLETED            10001
#define EVENT_ANIMATION_COMPLETED       10002
#define EVENT_ANIMATION_HALF_COMPLETED  10003
#define EVENT_ON_WILL_SHOW              10004
#define EVENT_ON_SHOW                   10005

#define EVENT_ON_YES                    10010
#define EVENT_ON_NO                     10011

// Global events
#define EVENT_TRS_UPDATED               50001
#define EVENT_OPEN_URL                  50002
#define EVENT_FACEBOOK_STATUS_UPDATED   50003
// when you receive this event - you may call CUser::Get()->GetIsFacebook()
// to fetch current status
#define EVENT_BOOSTERS_UPDATED          50004

#define EVENT_APP_ENTER_BACKGROUND      50101
#define EVENT_APP_ENTER_FOREGROUND      50102
#define EVENT_APP_PUSHES_UPDATED        50103

// Custom Events
#define EVENT_CUSTOM                   100000

// зарезервированные евенты
enum EEvents {/*eeTouchBegin, eeTouchMove, eeTouchEnd,*/ eeCount};

// структура данных тача
struct STouchData
{
    intptr_t ID;
    int type;
    vec2 pos;
    
    STouchData() { Reset(); }
    STouchData(intptr_t _id, int _type, vec2 _pos) : ID(_id), type(_type), pos(_pos) {}
    
    void SetParams(intptr_t _id, int _type, vec2 _pos) { ID = _id; type = _type; pos = _pos; }
    void Reset() { ID=0; type=-1; pos=vec2(); }
};

// структура данных для обработки евента показа кадра
class CAnimation;
struct SFrameShowData
{
    int frameNum;
    CAnimation* anim;
    SFrameShowData(int frame, CAnimation* anim):frameNum(frame), anim(anim){}
};

// структура данных евентов кнопки
struct SButtonEventData;

#pragma mark Event

class EventHandler;

class CEvent
{
public:
    CEvent();
    CEvent(const CEvent& event);
    ~CEvent();
    
    CEvent& operator=(const CEvent& event);
    
    void AddEventListener(EventHandler* listener);
    size_t  GetListenersCount() const { return m_attachedNodes.size(); }
    void RemoveListener(EventHandler* listener);
    void RemoveAllListeners();
    void RunEvent(void* data = NULL);
    void RunAndRemoveAllListeners(void* data = NULL);
    unsigned long GetID() const { return m_id; }
    void operator()(void* data = NULL);
    
private:
    std::vector<EventHandler*> m_attachedNodes;
    unsigned long m_id;
};

#pragma mark EventHandler

class EventHandler
{
protected:
    typedef void (EventHandler::*hnd)(void*);
    using lambda_pair = std::pair<unsigned long, std::function<void(void*)>>;
    using lambda_func = std::function<void(void*)>;
    
public:
    EventHandler();
    EventHandler(const EventHandler& ev);
    
    virtual void CopyEventHandler(unsigned long eventFrom, CEvent* eventTo); // копирует обработчик события из одного обьекта события в другой и добавляет его в список обработчиков нода
    virtual void RemoveEventListen(CEvent* ev, bool removeListener = true); // отключает прослушку данного события
    virtual void EventProcess(CEvent* ev, void* params);
    virtual bool IsDelete(); // check flag if node will be deleted in next CheckDelete() call
    CEvent* GetEventWithId(tEventIdType eventId);   // get event from map by its ID
    void RemoveEventById(tEventIdType eventId);  // remove event from map by its ID
    
    EventHandler* RemoveHandlerForEvent(tEventIdType eventId, EventHandler* obj);
    EventHandler* RemoveHandler(EventHandler* obj);
    
    void RemoveAllEventListen();			// отключает элемент от прослушки всех событий
    template<class T> void	AddEventHandler(CEvent& ev, void (T::*_hnd)(void*))	// Добавить обработчик события
    {																			// вызывать эту функцию для добавления обработчика на данное событие
        ev.AddEventListener(this);
        m_handlers.push_back(std::pair<unsigned long, hnd>(ev.GetID(), (hnd)_hnd));
    };
    
    template<class T> void	AddUniqueEventHandler(CEvent& ev, void (T::*_hnd)(void*))	// Добавить обработчик события ( will be called only one time )
    {																			// вызывать эту функцию для добавления обработчика на данное событие
        ev.AddEventListener(this);
        std::list< std::pair<unsigned long, hnd> >::iterator it = m_handlers.begin();
        while (it != m_handlers.end())
        {
            if (it->first == ev.GetID() && it->second == (hnd)_hnd)
                return;
            ++it;
        }
        m_handlers.push_back(std::pair<unsigned long, hnd>(ev.GetID(), (hnd)_hnd));
    };
    
    void AddEventHandler(CEvent &ev, EventHandler* lambda_owner, lambda_func);
    
    // Add handler to specified event
    template <class T>
    EventHandler* AddHandlerForEvent(tEventIdType eventId, T* obj, void (T::*handler)(void*)) {
        std::map<int, CEvent>::iterator search = m_events.find(eventId);
        if (search == m_events.end())
            m_events[eventId];
        obj->AddEventHandler(m_events[eventId], handler);
        return this;
    }
    
protected:
    bool m_isDelete;
    std::list< std::pair<unsigned long, hnd>>	m_handlers;     // list of handlers with event identifiers
    std::list< lambda_pair > m_lambdas;
    std::map<tEventIdType, CEvent>   m_events;   // list of events
};
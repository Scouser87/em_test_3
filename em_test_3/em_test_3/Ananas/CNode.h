/*
 *  CNode.h
 *  Ananas
 *
 *  Created by Anton Zagovalko on 24.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "EventHandler.h"
#include <list>
#include <map>
#include <string>

#define CHECK_DELETE_IMPROVEMENT 1

#define SAFE_DELETE_NODE(a){if(a){ a->Delete(); a = NULL;}}

class CAction;

enum ETochType { ettBegin, ettMove, ettEnd, ettCancel };

class CNode : public EventHandler
{
public:
							CNode();
							CNode(const CNode& node);                   // конструктор копирования
	virtual					~CNode();
	
    template<class T>                                                   // выставляет обработчика на вызов Delete()
	CNode*			        SetOnDeleteHandler(T* obj, void (T::*handler)(void*)) { obj->AddEventHandler(m_deleteEvent, handler); return this; }
    
    CNode*                  SetOnDeleteHandler(EventHandler* obj, lambda_func handler) { obj->AddEventHandler(m_deleteEvent, obj, handler); return this; }
    
    CEvent&				    GetEventOnDelete();                         // возвращает событие, происходящее при вызовe Delete()
    void					Delete();                                   // при необходимости удалить элемент вызвать эту функцию
    
#ifdef CHECK_DELETE_IMPROVEMENT
private:
    void                    DeleteWithChildren();
    
public:
#endif
	void					Delete(void*)       {Delete();}             // delete method for event handler
    virtual CNode*			AddChild(CNode* node);                      // добавляет потомка
	virtual CNode*			AddChild(CNode* node, int z);               // добавляет потомка с порядковым номером z
	virtual CNode*			RemoveChild(CNode* node, bool isDelete = true); // удаляет потомка если isDelete = true то удаляет объект иначе только ссылку
	virtual CNode*			RemoveAllChild();                           // удаляет всех потомков и их объекты
    static void             Swap(CNode* child1, CNode* child2);         // change Z order of nodes, nodes should have same parent
	
    bool                    IsForcedActionUpdate() { return m_forceActionsUpdate; }
    virtual void            EnableForceActionUpdate( const bool enableForceActionUpdate ) { m_forceActionsUpdate = enableForceActionUpdate; } // принудительное обновление Actions, если родитель на паузе
	virtual	void			Update(float dt){}		// обновление данного элемента
	virtual void			Simulate(float dt);		// обновление всех потомков
    void                    CheckDelete();          // recursive checking for nodes, which should be deleted
#ifdef CHECK_DELETE_IMPROVEMENT
    static void             DeleteFromMemory(CNode* node);
#endif
	virtual void			Visit();				// отрисовка элементов по иирархии (не перегружать!) Определено в CSprite
	virtual CNode*			SetPause(bool pause);
    void                    SetPauseOn (void* p=NULL) {SetPause(true);}
    void                    SetPauseOff(void* p=NULL) {SetPause(false);}
    bool                    IsPause(bool askParent = true);
	
    
    void                    SetTouchesON(void*) {SetIsTouchesOn(true);}
    void                    SetTouchesOFF(void*) {SetIsTouchesOn(false);}
    
    CNode*                  SetIsTouchesOnRecursive(bool);                      //вкл,выкл обработку тачей рекурсивно по вложености
    CNode*                  SetIsSingleTouch(bool);                             // устанавливает спрайт как обработчик только одного тача (остальные тачи не обрабатыватся и не передаются его потомкам)
    CNode*                  SetIsTouchesOn(bool);                               // включает/выключает обработку тачей для элемента и его потомков (по умолчанию включено)
    CNode*                  SetUserData(void* userData);                         //хранение пользовательской информации (внимание! удаляется автоматом)
    bool                    GetIsSingleTouch()const;
    bool                    GetIsTouchesOn()const;
	virtual	bool			TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed);	// функции для перегрузки
	virtual	bool			TouchMove(intptr_t ID, vec2& pos, bool isPrecessed);		// Обработчики тачей возвращает true если данный элемент обработал тач (если не нужно чтобы другие элементы его обрабатывали)
    virtual	bool			TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed);		// isPrecessed - true если тач уже был кем то обработан
    virtual void            TouchCancel(intptr_t ID, vec2& pos);
    virtual void            TouchReset(intptr_t ID, vec2& pos);                      // вызываеться при отмене тачей, для всех детей
    bool					TouchProcess(ETochType type, intptr_t ID, vec2& pos, bool isPrecessed); // вызывается системой
	
	CNode*					SetZOrder(int z);		// взять порядковый номер элемента в отрисовке
	int						GetZOrder() const;		// установить .........
	
	CNode*					GetParent() const;
    
    //bool                    IsIterateChildren() {return m_isIterateChildren;}
    
	std::list< CNode* >&	GetChildren();
    CNode*                  GetChildByZ(int z);
    CNode*                  GetChildByZ(const std::list<int>& zIndex);
    CNode*                  GetFirstChild();
    CNode*                  GetPrevSibling();
    CNode*                  GetNextSibling();
	
	bool					IsNodeIncluded(CNode* node) const;  // проверяет подключен ли элемент node к данному или его потомкам
	
	void					SetName(const std::string& name);  // имя элемента (для удобства отладки)
	const std::string&		GetName() const;
    void*                   GetUserData();
    
	virtual bool	GetIsHUD()const;			// for CSprite, to checking if this sprite is HUD
    bool            GetIsHUDThisOrParent(); // check if node or it's parent is HUD
	
	virtual CNode*			GetCopy();	// при наследовании нужно определить эту функцию, чтобы элемент можно было копировать когда копируется контейнер в котором он лежит
    
    template<class T> T*    AsCopy(T* original) //присвоить всем указателям в функции GetCopy(), находит указятель в новой иерархии
    {
        return dynamic_cast<T*>(FindAccordantByZ(original));
    }

    
    // возвращает лист родителей(родословную )  начиная от progenitor
    std::list<CNode*> GetParentTree(const CNode* progenitor);
    std::list<int>    GetIndexChild(const CNode* progenitor); // oт progenitor до this(включительно) 
    
    CNode*          AddAction(CAction*);	// функция добавляет экшен в менеджер экшенов и связывает его с текущим спрайтом
    void            ClearActions();
    bool            HasActions()const{return m_hasActions;}
    void            SetHasActions(bool b){m_hasActions=b;}
    
    CNode*                  GetSubNodeByName(std::string name); // get first child with matching name, recursively searching
    template<class T> T*    GetSubNodeByName(std::string name) // get first child with matching name with casting to needed type, recursively searching
    {
        return dynamic_cast<T*>(GetSubNodeByName(name));
    }
    
protected:
    virtual CNode*                   FindAccordantByZ(CNode* original) {return original ? GetChildByZ((original)->GetZOrder()) : NULL;}
    virtual void                     CheckParentsPause(); // проверить стоит ли какой-либо родитель на паузе
    bool                             m_isParentPause;     // стоит ли какой-либо родитель на паузе

private:
    void                    TouchResetSys(intptr_t ID, vec2& pos);
	std::list< CNode* >		m_children; // list of node children
	CNode*					m_parent;   // node's parent
    
	bool					m_isPause;  // flag (is used to check should this node be updated or not)
    
	std::string				m_name;     // node's string name identifier
    
    intptr_t                m_singleTouch;  // flag (if true - node will handle only one touch)
    bool                    m_isTouchOn;    // flag (if false - node won't handle any touches)

    CEvent		            m_deleteEvent;  // event which will be called before deleting of this node
        
    bool                    m_hasActions;   // flag (to check if node has attached actions)
    bool                    m_forceActionsUpdate; // update actions even if parent on pause

	friend class InteractiveCorrector;
    
    void*                   m_userData;     //хранение пользовательской информации
    
//////////////////////////////// Nodes count logic ////////////////////////////////
private:
    static unsigned long long s_nodeCounter;
    static unsigned long long s_nodeUpdatedCounter;
    static bool s_shouldCountNodes;
public:
    static void SetCountNodes(bool value);
    static const unsigned long long& GetNodeCount();
    static void ResetNodeUpdatedCount();
    static const unsigned long long& GetNodeUpdatedCount();
//////////////////////////////// Nodes count logic end ////////////////////////////////
};

// Macros for loop thru CNode
// pt - pointer to CNode
// it - iterator
//  usage example:
//  NODE_LOOP(node, it)
//  {
//    ((CNode*)(*it))node->DoSmth();
//  }
#define NODE_LOOP(pt, it) std::list< CNode* > children = pt->GetChildren(); for(std::list< CNode* >::iterator it = children.begin(); it != children.end(); ++it)

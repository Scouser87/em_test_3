/*
 *  CButton.h
 *  Casino
 *
 *  Created by Anton Zagovalko on 30.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "CSprite.h"

class CActionSequence;
class CButton;

//////////////// структура данных евентов кнопки ///////////////
struct SButtonEventData 
{
	enum EType {etSelect, etUnselect, etPress};
	
	CButton*	button;		// указатель на кнопку
	EType		type;		// тип воздействия
    vec2        touchPos;   // позиция тача в глобальных координатах
    void*       userData;    // пользовательские данные
	
	SButtonEventData(CButton* _button, EType _type, vec2 touchPos, void* userData): button(_button), type(_type), touchPos(touchPos), userData(userData){}
};

 
//////////// класс кнопки ////////////////////
class CButton: public CSprite
{
public:
						CButton();
						CButton(CSprite* normal, vec2 shift = vec2());                              // создание кнопки из одной картинки со смещением при нажатии
                        CButton(CSprite* normal, CSprite* pressed, CSprite* disabled = NULL);       // создание кнопки из 2х или 3х картинок
						CButton(const CButton& but);
                        ~CButton();
	
	CButton*			SetNormal(CSprite* normal,bool delActions=true);                                                 // установка нормального состояния
	CButton*			SetPressed(CSprite* pressed,bool delActions=true);                                               // установка нажатого состояния
	CButton*			SetDisabled(CSprite* disabled,bool delActions=true);                                             // установка спрайта неактивного состояния
	CButton*			SetPressedShift(vec2 shift);                                                // установка смещения кнопки при нажатии
	CButton*			SetPressedScale(vec2 scale);                                                // установка скалирования кнопки при нажатии
	CButton*			SetPressedScale(float scale);
    CButton*            SetOnOverButton(bool flag);                                                 // выставляет флаг на выделение кнопки
    CButton*            SetPressedRotate(float angle);                                              // устанавливает угол, на который будет поворачиваться изображение кнопки при нажатии
	CButton*			SetPressedRectScale(vec2 scale);                                             // устанавливает размер ректа для обработки нажатия от 0 до 1 (1 - размер картинки нормального состояния)
    CButton*			SetPressedRectSize(vec2 size);                                              // устанавливает размер ректа для обработки нажатия
    CButton*			SetPressedRectShift(vec2 shift);                                            // установка смещения ректа нажатия
    CButton*            SetPressedRectVisible(float r = 0.3f, float g = 0.0f, float b = 0.0f);      // включает/отключает отрисовку ректа нажатия.
	CButton*			SetSmoothSwich(float time);                                                 // установка плавного перехода к спрайту нажатия
	CButton*			SetEnable(bool isEnable);                                                   // установка активного / неактивного состояния
    CSprite*            SetSize(const vec2& size);
    CSprite*            SetAnchorPoint(const vec2&);
    CSprite*            SetIsAlphaIgnore(bool isIgnor);     // если установить, включается проверка на нажатие на непразрачное место картинки (кнопка не реагирует если нажимать на прозрачное место)
    
    CSprite*            GetNormal();                // возвращает спрайт нормального состояния
    CSprite*            GetPressed();               // возвращает спрайт нажатого состояния
    CSprite*            GetDisabled();              // возвращает спрайт неактивного состояния
		
	CEvent&				GetEventOnSelect();			// возвращает событие, происходящее при выделении кнопки
    CEvent&             GetEventOnUnSelect();       // возвращает событие, происходящее при развыделении кнопки
	CEvent&				GetEventOnPress();			// возвращает событие, происходящее при нажатии (отпускании) кнопки
    
    bool                GetEnabled() { return m_isEnable; }
	
    vec2                GetPressedRectSize();
    vec2                GetPressedRectShift();
    
	template<class T>								// выставляет обработчика на нажатие 
	CButton*			SetOnPressHandler(T* obj, void (T::*handler)(void*))				{ obj->AddEventHandler(m_pressEvent, handler); return this; }
	template<class T>								// выставляет обработчика на выделение
	CButton*			SetOnSelectHandler(T* obj, void (T::*handler)(void*))				{ obj->AddEventHandler(m_selectEvent, handler); return this; }
	template<class T>								// выставляет обработчика на сброс выделения
	CButton*			SetOnUnSelectHandler(T* obj, void (T::*handler)(void*))				{ obj->AddEventHandler(m_unSelectEvent, handler); return this; }
    
    CButton*			SetOnPressHandler(EventHandler* obj, lambda_func handler)				{ obj->AddEventHandler(m_pressEvent, obj, handler); return this; }
    CButton*			SetOnSelectHandler(EventHandler* obj, lambda_func handler)				{ obj->AddEventHandler(m_selectEvent, obj, handler); return this; }
    CButton*			SetOnUnSelectHandler(EventHandler* obj, lambda_func handler)			{ obj->AddEventHandler(m_unSelectEvent, obj, handler); return this; }
    
    CButton*            SetData(void* data);        // добавление пользовательских данных в отправляемых обработчикам
    void*               GetData();                  //
    CNode*              SetIsTouchesOn(bool);
    
    void                RemoveButtonActions();
    void                RemoveButtonEvents();
	
	virtual CButton*	GetCopy();	// при наследовании нужно определить эту функцию, чтобы элемент можно было копировать когда копируется контейнер в котором он лежит
	
    
    bool		TouchBegin(intptr_t ID, vec2& pos, bool isPrecessed);
	bool		TouchMove(intptr_t ID, vec2& pos, bool isPrecessed);
	bool		TouchEnd(intptr_t ID, vec2& pos, bool isPrecessed);
    void        TouchCancel(intptr_t ID, vec2& pos);
    void        TouchReset(intptr_t ID, vec2& pos);

    virtual void        Unselect();
    void                SetNormalScaleAnim(bool value); // добавить анимацию пульсации для ненажатого состояния
    
    void                CheckPressedRect();
    
protected:
    
    void                AddNormalScaleAnim();
    void                RemoveNormalScaleAnim();
    
	void		SetIsPressed(bool);
    
    bool         DetectTouch(vec2 pos);
	
	CSprite*	m_normal;
    vec2        m_normalStartPos; // стартовая позиция нормального спрайта
	CSprite*	m_pressed;
    vec2        m_pressedStartPos; // стартовая позиция нажатого спрайта
	CSprite*	m_disable;
	CSprite*	m_pressDetecter;
    float*      m_alphaTouch;
    vec2        m_alphaSize;
	vec2		m_shift;
	vec2		m_pressedShift;
	vec2		m_scaleshift;
    float       m_angle;
	CEvent		m_selectEvent;
	CEvent		m_unSelectEvent;
	CEvent		m_pressEvent;
	bool		m_isSelect;
    bool        m_OnOverButton;
	intptr_t	m_tochID;
	float		m_swichTime;
	bool		m_isEnable;
    bool        m_isNormalScaleAnim;
	CActionSequence*	m_procActive;
	CActionSequence*	m_procNormal;
    void*       m_eventData;
    CAction*    m_normalScaleAnim;
};
 

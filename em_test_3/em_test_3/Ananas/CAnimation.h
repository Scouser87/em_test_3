/*
 *  CAnimation.h
 *  Ananas
 *
 *  Created by Anton Zagovalko on 27.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "CSprite.h"
#include "TexturePackerLoader.h"

// структура данных для обработки евента показа кадра
struct SFrameShowData;

// класс анимации
class CAnimation: public CSprite
{
public:
					CAnimation();
					CAnimation(CAnimation& anim);
					~CAnimation();

    void            ClearFrames(); // удалить все фреймы из анимации
	CAnimation*		AddFrame(std::string path); // добавление кадра из картинки
	CAnimation*		AddFrame(std::string pathMask, int from, int to); // добавление кадров из папки по маске в стиле printf (нужно вставить в маску %i)
    CAnimation*     AddFrameXML( std::string pathToXMLFolder); // добавление кадров из XML после кропера, на конце должен быть /
    CAnimation*     AddFrameXMLPercent( std::string pathToXMLFolder, float framePercent = 1.0f); // добавление кадров из XML после кропера c указанием процента добавляемого кол-ва кадров, на конце должен быть /
	CAnimation*		AddFrame(std::string path, vec2 shift); // добавление кадра со смещением центра
	CAnimation*		AddFrame(CImage* texture, vec2 from, vec2 to, vec2 shift);                                             // добавление кадра из части картинки (параметры задаются от 0 до 1)
	
    CAnimation*		AddFrame(CImage* texture, sFrameInfo info, sMetaData metadata);                                             // добавление кадра из части атласа, созданного TexturePacker

	CAnimation*		SetAnimationSpeed(float fps);				// установка скорости анимации (количество кадров в секунду)
	CAnimation*		SetAnimationPlayTime(float time);			// установка времени проигрывания всей анимации
	CAnimation*		SetFrameStartEvent(CEvent ev, int frame);	// добавление события на установку определенного кадра
																// для события добавления события на конец анимации ставить на последний кадр +1 (можно получить через GetFrameNum())
	template<class T>											// на нулевой кадр нельзя прицепить событие
	CAnimation*		SetFrameStartHandler(int frame,  CNode* obj, void (T::*handler)(void*)) /* добавления обработчика на появление кадра */			{ if (m_frameEvents.find(frame) == m_frameEvents.end()) m_frameEvents[frame] = CEvent(); obj->AddEventHandler(m_frameEvents[frame], handler);return this; }
    void            ClearFrameEvents() { m_frameEvents.clear(); }
	
    int				GetFrameNum();
	
    void            OnRunAnimation(void*)                      { RunAnimation(IsCycle());}
	CAnimation*		RunAnimation(bool isCycle, int frame = 0);	// запуск анимации. можно начать с любого кадра
	CAnimation*		StopAnimation();
    
	CAnimation*		SetFrame(int frame);                        // установить кадр анимации
	
	void			Update(float dt);	

	virtual CAnimation*	GetCopy();	// при наследовании нужно определить эту функцию, чтобы элемент можно было копировать когда копируется контейнер в котором он лежит
	
    int             GetCurrentFrame() { return m_currentFrame; }    // номер текущего кадра начиная с 0
    bool            IsPlay() { return m_isPlay; }                   // проигрывается ли анимация    
    bool            IsCycle() { return m_isCycle; }                   // проверить зациклена ли анимация    
    float           GetAnimationSpeed() { return 1.0f/m_frameTime; }    //скорость анимации (количество кадров в секунду)

    CImage*         GetFrameImageByNum(int frameNum);       // изображение для указанного кадра
    vec2            GetFrameShiftByNum(int frameNum);       // смещение для указанного кадра

    void            Reverse();  // переворачивает анимацию, первый кадр становится последним

    
    void            ShowAnimation(void*); // показать анимацию
    void            HideAnimation(void*); // скрыть анимацию
    
    void            Active(); //Активирует (прогружает) анимацию
    
private:
    void            CallEvent();
	void			SetFrame();
    struct SAnimationFrame;
	
	std::vector< SAnimationFrame* >		m_frames;
	int									m_currentFrame;
	float								m_time;
	float								m_frameTime;
	bool								m_isCycle;
	std::map< int, CEvent >				m_frameEvents;
	bool								m_isPlay;


	// обычный кадр
	struct SAnimationFrame	
	{
	public:
                                    SAnimationFrame(std::string imagePath);
                                    SAnimationFrame(CImage* img);
        virtual                     ~SAnimationFrame();
		virtual bool				SetFrameTo(CSprite*);

		virtual	SAnimationFrame*	GetCopy() { return new SAnimationFrame(*this); }

        CImage*                     GetImage() { return m_image; }
        void                        SetImage(const std::string& imagePath);
		
	private:
		CImage* m_image;
	};

	// кадр со смещением
	struct SAnimationFrameShift : public SAnimationFrame	
	{	
	public:
									SAnimationFrameShift(std::string imagePath, vec2 shift):SAnimationFrame(imagePath), m_shift(shift) {}
                                    SAnimationFrameShift(CImage* img, vec2 shift) : SAnimationFrame(img), m_shift(shift) {}
                                    ~SAnimationFrameShift(){}
		virtual bool				SetFrameTo(CSprite*);
		virtual	SAnimationFrame*	GetCopy() { return new SAnimationFrameShift(*this); }

        vec2                        GetShift() { return m_shift; }
		
	protected:
		vec2 m_shift;
	};

	// кадр - рект из текстуры
	struct SAnimationFrameTexturePart : public SAnimationFrameShift	
	{	
	public:
		SAnimationFrameTexturePart(CImage* texture, vec2 from, vec2 to, vec2 shift)
			:SAnimationFrameShift(texture, shift), 
			 m_from(from), 
			 m_to(to) {}
        ~SAnimationFrameTexturePart(){}
		virtual bool				SetFrameTo(CSprite*);
		virtual	SAnimationFrame*	GetCopy() { return new SAnimationFrameTexturePart(*this); }
		
	private:
		vec2 m_from;
		vec2 m_to;
	};
    
    struct SAnimationFrameTexturePacker : public SAnimationFrame
    {
    public:
		SAnimationFrameTexturePacker(CImage* texture, sFrameInfo info, sMetaData metadata): SAnimationFrame(texture), mFrameInfo(info), mMetaData(metadata) {}
        ~SAnimationFrameTexturePacker(){}
		virtual bool				SetFrameTo(CSprite*);
		virtual	SAnimationFrame*	GetCopy() { return new SAnimationFrameTexturePacker(*this); }
		
	private:
        sFrameInfo mFrameInfo;
        sMetaData  mMetaData;
    };

};

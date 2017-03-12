/*
 *  CSprite.h
 *  Ananas
 *
 *  Created by Anton Zagovalko on 24.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Sprite_hpp
#define Sprite_hpp

#include "CNode.h"
#include "CImage.hpp"
#include "sColor.h"
#include <GLES2/gl2.h>
#include <GLES/gl.h>

enum EBledMode      { ebmNormal=0, ebmAdd, ebmMultiply, ebmScreen, ebmCustom };
enum EFlip		    { efNone, efHorizontal, efVertical, efBoth };
enum EPlaceStyle    { epsLine, epsColomn };
enum eFBORenderMode { eFBO_RM_Normal = 0, eFBO_RM_Add, eFBO_Non_Update };

class CFrameBufferObject;
class CSpriteExternal;

class CSprite: public CNode
{
    friend CSpriteExternal;
public:    
    struct sBlendParams
    {
        GLenum srcRGB;
        GLenum dstRGB;
        GLenum srcAlpha;
        GLenum dstAlpha;
        sBlendParams()
        {
            srcRGB = GL_SRC_ALPHA;
            dstRGB = GL_ONE_MINUS_SRC_ALPHA;
            srcAlpha = GL_SRC_ALPHA;
            dstAlpha = GL_ONE_MINUS_SRC_ALPHA;
        }
        void Set(const GLenum& _srcFactor, const GLenum& _dstFactor)
        {
            srcRGB = _srcFactor;
            dstRGB = _dstFactor;
            srcAlpha = _srcFactor;
            dstAlpha = _dstFactor;
        }
        void Set(const GLenum& _srcRGB, const GLenum& _dstRGB, const GLenum& _srcAlpha, const GLenum& _dstAlpha)
        {
            srcRGB = _srcRGB;
            dstRGB = _dstRGB;
            srcAlpha = _srcAlpha;
            dstAlpha = _dstAlpha;
        }
    };
    

    enum eFastAnchorPoint {
        EFAP_CENTER,
        EFAP_LEFT,
        EFAP_RIGHT,
        EFAP_TOP,
        EFAP_BOTTOM
    };
    
    enum EBlendColor{
        EBC_Multiply,
        EBC_Add,
        EBC_HSV
    };
    
						CSprite();
						CSprite(const std::string& imagePath);
						CSprite(CImage* image);
						CSprite(const sColor& color);
						CSprite(const CSprite& sprite);		// конструктор копирования
						~CSprite();
	
    virtual void		Simulate(float dt);
	virtual void		Draw();		// отрисовка элемента
	virtual void		Visit();	// отрисовка дочерних элементов
	virtual CSprite*	SetVisible(const bool& visible);
    void                SetVisibleOn(void*) { SetVisible(true);}
    void                SetVisibleOff(void*){ SetVisible(false);}
    
    virtual bool        BeforeVisit() { return false;}
    virtual bool        AfterVisit()  { return false;}
   
    
    CSprite*            SetVisibleAll(const bool& visible); // устанавливает выдимость для этого и всех дочерних элементов
    CSprite*            SetActive(const bool& isActive);    // отключает и включает отрисовку и обновление спрайта (делает его активным или не активным)
    void                SetActiveOn(void*)           { SetActive(true);}
    void                SetActiveOff(void*)          { SetActive(false);}
	CSprite*			SetIsHUD(bool isHUD);		 // назначает текущий элемент элементом игрового меню (HUD) этот элемент не двигается при движении камеры
	
    
    bool                GetIsActive();
	CImage*				GetImage();
    bool                IsPremultipliedAlpha() {return m_isPremultipliedAlpha;}
	const vec2&			GetPosition()const;
	const vec2&			GetSize()const;
	const vec2&			GetScale()const;
	const vec2&			GetAnchorPoint()const;
	const float&		GetAngle()const;
	const sColor&		GetColor()const;
          EBlendColor	GetBlendColor()const{return m_blendColor;}
	const float&		GetOpacity()const;
          bool			IsVisible(bool checkParents = true);    // checkParents - проверка на видимость родителей спрайта
    const EFlip&		GetFlip()const;
	const EBledMode&	GetBlendMode()const;
    const sBlendParams& GetBlendParams()const;
	const float*     	GetVertex() const;
          float*     	GetVertex();
	const float*        GetTextCoord() const;
    virtual bool		GetIsHUD()const;
	      bool			GetIsScissor()const;
    static vec2			GetScreenSize();
	vec2				GetPositionScreen(); //получение позиции в экранных координатах
    bool                GetIsTexCoordRotated()const{return m_texCoordRotated;}
//    sColor*             CreateSpriteTextRect(int& width, int& height);  // поулчение массива точек которые рисует спрайт
	
	virtual CSprite*	SetImage(CImage*);
	virtual CSprite*	SetImage(const std::string&);
	virtual CSprite*	SetTextureCoord(const vec2& from, const vec2& to);	// текстурные координаты левого верхнего и првавого нижнего угла. Значения ставятся от 0 до 1
    CSprite*            TextureCoordRotate90();// поворот на 90 градусов
    void                SetPremultipliedAlpha(bool isPremultipliedAlpha);
	virtual CSprite*	SetPosition(const vec2&);
	virtual CSprite*	SetPositionBy(const vec2&);    // сдвиг на
	virtual CSprite*	SetSize(const vec2&);			// резмер картинки или залитого прямоугольника (не изменяет размер вложенных элементов)
    virtual CSprite*    SetSizeWithCrop(const vec2&);  // изменяет размер элемента, не растягивает содержимое (в отличие от SetSize)
	virtual CSprite*	SetScale(const vec2&);			// скалирует текущий и вложенные элементы относительно якоря
#if OS == OS_WP8
	CSprite*			SetScaleVec2(const vec2&);			// скалирует текущий и вложенные элементы относительно якоря
#endif
	virtual CSprite*	SetScale(const float&);		//
    virtual CSprite*    SetAnchorPoint(const int&);    // быстрая установка якоря из предопределенного списка.
	virtual CSprite*	SetAnchorPoint(const vec2&);
	virtual CSprite*	SetAngle(const float&);		// относительно якоря
            void        SetBlendColor(EBlendColor blendColor) {m_blendColor = blendColor;}
            CSprite*	SetColor(const sColor&, EBlendColor blendColor);
	virtual CSprite*	SetColor(const sColor&);		// выставляет цвет только для этого элемента
            void        SetHSV(const float& h = 0.0f, const float& s = 0.999f, const float& v = 0.999f);
	virtual CSprite*	SetOpacity(const float& op);		// выставляет прозрачность для этого элемента и его потомков
            void        SetChangeChildrenOpacity(bool state) { m_changeOpacityForChildren = state; };
            bool        GetChangeChildrenOpacity() { return m_changeOpacityForChildren; }
	virtual CSprite*	SetBlendMode(const EBledMode&);
    virtual CSprite*    SetBlendParams(const sBlendParams& blendParams);
	virtual CSprite*	SetFlip(const EFlip&);
	virtual CSprite*	SetIsScissor(const bool& isScissor); // устанавливает будет ли спрайт и его содержимое обрезаться по размеру спрайта
	
	void				DrawAfter(CSprite* sprite, bool pushBack = true);					// данный элемент будет рисоваться после спрайта sprite
	void				DontDrawAfter();							// удаляет данный элемент из списка отображения другого элемента
	void				RemoveAllDrawAfter();						// удаляет все элементы которые привязаны к его списку отображения
    
											// распологает дочерние элементы в линию или столбец с промежутками space
											// элементы распологаются от точки (0,0) в зависимости от установленного anchorPoint (anchorPoint = (0, 0) - от (0,0) вправо для epsLine и вверз для epsColomn)
											// alignPoint - место относитльно которого выравниваются элементы (0.5 - ровнение по центру, 0 - по левому краю для epsColomn, по низу для epsLine
	CSprite*			PlaceObjectsAs(EPlaceStyle style, float space = 10, float alignPos = 0.5);
    CSprite*            PlaceObjectsInRow(float spaceY = 0.0f);     // располагает дочерние элементы в строку,
                                                                    // переносит элементы на следующую строку если все не вписались по ширине спрайта
                                                                    // если нужно задать фиксированное расстояние между элементами по высоте используем spaceY
	CSprite*			PlaceObjectsAs(fSRectHW SizeRect, vec2 space);
	vec2				GetGlobalFromLocalCoord( const vec2 &_pos );	// Преобразует локальные координаты объекта в глобальные
    vec2				GetLocalFromGlobalCoord( const vec2 &_pos );	// Преобразует глобальные координаты объекта в локальные
    bool				IsPointInSprite( const vec2& _point );			// Проверяет попадает ли точка в спрайт. 
																		// Точка в глобальных координатах/
    const float*        GetMatrixSave() const {return m_matrixSave;}

    bool                IntersectWithCamera();                          // Проверка пересечения с камерой
    
//    CSprite*            Correct(std::string name = "");              // включить корректировку позиция спрайта
//    CSprite*            CorrectSize(std::string name = "");          // включить корректировку размераы спрайта
//    CSprite*            CorrectScale(std::string name = "");          // включить корректировку маштаба спрайта
//    CSprite*            CorrectHSV(std::string name = "");          // включить корректировку компонент HSV
//    CSprite*            CorrectOpacity(std::string name = "");
    
	CNode*				AddChild(CNode* node);
	CNode*				AddChild(CNode* node, int z);
    
	virtual CSprite*	GetCopy();	// при наследовании нужно определить эту функцию, чтобы элемент можно было копировать когда копируется контейнер в котором он лежит
    
    vec2                GetTextureCoordFrom(){return m_textureCoordFrom;}
    vec2                GetTextureCoordTo(){return m_textureCoordTo;}

virtual
	void				SetMatrix();		// устанавливает матрицу вида текущего элемента и делает ее текущей в OpenGl (обязательно заключать в glPushMatrix() - glPopMatrix())
	void				SetMatrixRecur();	// устанавливает матрицу вида текущего элемента и его потомков
	void				ResetMatrix();		// обнуляет матрицу вида текущего элемента
    bool                IsMatrixSet()const{return m_isMatrixSet;}
    
    float               GetOpacityWithParent();
    
    void                DisableRenderWithFBO();
    void                EnableRenderWithFBO(eFBORenderMode mode = eFBO_RM_Normal); // включить для этого спрайта и его копий отрисовку через FrameBufferObject
    void                EnableRenderWithFBO(int ID, eFBORenderMode mode = eFBO_RM_Normal); // включить для этого спрайта и его копий отрисовку через FrameBufferObject
    void                SetNotCalculationToFBO(bool val);
    void                PrepareRenderToFBORecur();
    virtual void        PrepareRenderToFBO();

    void                SetRenderToFBO(bool val);
    void                SetOnlyGenerateFBO(bool val);
    
    virtual void        SetFBORenderMode(int mode);

    void                EnableStencilForFBO();
    
    void                EnableRenderProfiling(bool value, std::string name="");

    bool                IsNotCalculationToFBO()const {return m_isNotCalculationToFBO;}
    bool                IsRenderToFBO()const {return m_isRenderToFBO;}
    
protected:
    virtual void        SetOpacityChild(float op);

    const int* GetSaveScissor()const    {return m_saveScissor;}
    void       SaveScissor();
    void	   EnableScissor(bool isRotated);
    void	   DisableScissor(bool isRotated);
    void	   DrawAfterRun();
    
    bool                    m_isNotCalculationToFBO;
    bool                    m_isRenderToFBO;
    bool                    m_isOnlyGenerateFBO;

    CFrameBufferObject*     m_fbo;
    int                     m_fboID;
private:
    virtual void    CountMatrix();
    
    void            RecalcOpacityWithParent();
    
    void            UpdateBlendParams();
    
    // returns TRUE if sprite is on the screen
    //         stack is filled with sprite's parent sprites
    bool            IsOnScreen(std::vector<CSprite*>& stackOfParents);
    void            RecalculateMatrixAndUpdateParents(std::vector<CSprite*>& stackOfParents);

	void			SetVertex();
	void			SetTexCoord(const vec2& p = vec2(0, 0), const vec2& to = vec2(1, 1));
	
    void            Render();
	//virtual void	SetScissor();
	
	CImage*		m_Texture;
    bool        m_isPremultipliedAlpha;
	vec2		m_pos;
	vec2		m_size;
	vec2		m_scale;
	vec2        m_anchorPoint;
	float		m_angle;
	EFlip		m_flip;
	sColor		m_color;
    EBlendColor m_blendColor;
	float		m_opacity;
    float		m_opacityWithParent;
	bool		m_isVisible;
	float*		m_vertex;
	float*		m_textCoord;
	float*		m_matrixSave;
    float*      m_HSV;
	bool		m_isMatrixSet;
	bool		m_isHUD;
    
	bool		m_isScissor;
    bool		m_isSaveScissor;
    int*        m_saveScissor;
    
    vec2       m_textureCoordFrom;
    vec2       m_textureCoordTo;

    EBledMode	 m_blendMode;
    sBlendParams m_blendParams;
    
    bool        m_texCoordRotated;

    bool            m_enableRenderProfiling;
    std::string     m_profilingName;

	CSprite*	m_drawAfterSprite;
	std::list< CSprite* >	m_drawAfterSpriteList;	// список элементов которые будут рисоваться после данного
    
	
	friend class InteractiveCorrector;
    
    bool        m_needRecalcOpacityWithParent;
    bool        m_changeOpacityForChildren;
};

#endif /* Sprite_hpp */

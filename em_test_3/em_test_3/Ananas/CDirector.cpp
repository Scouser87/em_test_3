/*
 *  CDirector.cpp
 *  Ananas
 *
 *  Created by Anton Zagovalko on 25.01.12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "CDirector.h"

#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>
#include <sys/time.h>
#include <time.h>
#include <queue>
#include "algebra3.h"
#include "Actions/CAction.h"
#include "CNode.h"

CDirector* CDirector::s_director = NULL;

CDirector::CDirector()
:m_prevTime(0)
{
    m_root = new CNode;
    m_actionMngr = new CActionMngr;
    CDirector::s_director = this;
}

CDirector::~CDirector()
{
    delete m_root;
    delete m_actionMngr;
}

CDirector& CDirector::GetDirector()
{
    if (s_director == NULL)
    {
        s_director = new CDirector;
    }
    return *s_director;
}

CActionMngr& CDirector::GetActionManager()
{
    return *GetDirector().m_actionMngr;
}

void CDirector::Init(float x, float y)
{
    m_canvasSize.x = x;
    m_canvasSize.y = y;
    
    emscripten_set_canvas_size(m_canvasSize.x, m_canvasSize.y);
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = attr.depth = attr.stencil = attr.antialias = attr.preserveDrawingBuffer = attr.preferLowPowerToHighPerformance = attr.failIfMajorPerformanceCaveat = 0;
    attr.enableExtensionsByDefault = 1;
    attr.premultipliedAlpha = 0;
    attr.majorVersion = 1;
    attr.minorVersion = 0;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(0, &attr);
    emscripten_webgl_make_context_current(ctx);
//    
//    LoadShaders();
//    LoadShaderParams();
//    
//    m_mouseHandler = new MouseHandler;
}

static std::queue<STouchData> s_actionTouches;
static bool s_isResetTouches = false;

void CDirector::Update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float dt = CalculateDT();
    
    ETochType type = (ETochType)s_actionTouches.front().type;
    if(s_isResetTouches)
        type = ettCancel;
    vec2& pos = s_actionTouches.front().pos;

    pos.y = m_canvasSize.y - pos.y;

//    pos = CCamera::GetCamera().ConvertTouchToWorldCoord(pos);
    m_root->TouchProcess(type, s_actionTouches.front().ID, pos, false);
    s_actionTouches.pop();
    
    m_root->Simulate(dt);
    m_actionMngr->Update(dt);
    
    if (!m_nodesToDelete.empty())
        CheckDeleteNodes();
    
    m_root->Visit();
}

void CDirector::TouchBegin(intptr_t ID, vec2 pos)
{
    
}

void CDirector::TouchMove(intptr_t ID, vec2 pos)
{
    
}

void CDirector::TouchEnd(intptr_t ID, vec2 pos)
{
    
}

void CDirector::TouchCancel(intptr_t ID, vec2 pos)
{
    
}

void CDirector::ResetTouches()
{
    
}

void CDirector::SetScreenSize(vec2 size)
{
    
}

void CDirector::DeleteNode(CNode* node)
{
    m_nodesToDelete.insert(node);
}

void CDirector::CheckDeleteNodes()
{
    std::set<CNode*> deleteNodesCopy = m_nodesToDelete;
    m_nodesToDelete.clear();
    
    for (std::set<CNode*>::const_iterator it = deleteNodesCopy.begin(); it != deleteNodesCopy.end(); ++it)
    {
        CNode* node = *it;
        if (CNode* parent = node->GetParent())
            parent->RemoveChild(node, false);
    }
    for (std::set<CNode*>::const_iterator it = deleteNodesCopy.begin(); it != deleteNodesCopy.end(); ++it)
    {
        CNode* node = *it;
        node->CheckDelete();
        CNode::DeleteFromMemory(node);
    }
    deleteNodesCopy.clear();
}

static float prevDT = 1.0/60.0;

double CDirector::CalculateDT()
{
    struct timezone tz;
    struct timeval t;
    gettimeofday(&t, &tz);
    double tim = double(t.tv_sec) + (t.tv_usec / 1000000.0);
    double dt = tim - m_prevTime;
    m_prevTime = tim;
    if (dt > 0.2) dt = 0.2;// fps min
#ifndef __QT__
    if (dt < 0.0166666f) dt = 1.0f / 60.0f; // при переводе времени на час назад, dt = -300 где-то
#endif /* __QT__ */
    float d=fabs(prevDT-dt);
    const double delta_t=0.000003;
    if (d>delta_t)
    {
        if(prevDT>dt)    {prevDT-=d*0.1;dt=prevDT;}
        if(dt>prevDT)    {prevDT+=d*0.1;dt=prevDT;}
    }
    return dt;
}

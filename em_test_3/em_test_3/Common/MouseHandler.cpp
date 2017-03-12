//
//  MouseHandler.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 10.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include "MouseHandler.hpp"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <string.h>
#include "CDirector.h"

const char *emscripten_result_to_string(EMSCRIPTEN_RESULT result) {
    if (result == EMSCRIPTEN_RESULT_SUCCESS) return "EMSCRIPTEN_RESULT_SUCCESS";
    if (result == EMSCRIPTEN_RESULT_DEFERRED) return "EMSCRIPTEN_RESULT_DEFERRED";
    if (result == EMSCRIPTEN_RESULT_NOT_SUPPORTED) return "EMSCRIPTEN_RESULT_NOT_SUPPORTED";
    if (result == EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED) return "EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED";
    if (result == EMSCRIPTEN_RESULT_INVALID_TARGET) return "EMSCRIPTEN_RESULT_INVALID_TARGET";
    if (result == EMSCRIPTEN_RESULT_UNKNOWN_TARGET) return "EMSCRIPTEN_RESULT_UNKNOWN_TARGET";
    if (result == EMSCRIPTEN_RESULT_INVALID_PARAM) return "EMSCRIPTEN_RESULT_INVALID_PARAM";
    if (result == EMSCRIPTEN_RESULT_FAILED) return "EMSCRIPTEN_RESULT_FAILED";
    if (result == EMSCRIPTEN_RESULT_NO_DATA) return "EMSCRIPTEN_RESULT_NO_DATA";
    return "Unknown EMSCRIPTEN_RESULT!";
}

#define TEST_RESULT(x) if (ret != EMSCRIPTEN_RESULT_SUCCESS) printf("%s returned %s.\n", #x, emscripten_result_to_string(ret));

EM_BOOL mouse_down_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    if (e->screenX != 0 && e->screenY != 0 && e->clientX != 0 && e->clientY != 0 && e->canvasX != 0 && e->canvasY != 0 && e->targetX != 0 && e->targetY != 0)
    {
        MouseHandler* mHandler = (MouseHandler*)userData;
        mHandler->SetCanvasPos(e->canvasX, e->canvasY);
        mHandler->SetState(MouseHandler::eMS_down);
        CDirector::GetDirector().TouchBegin(1, vec2(e->canvasX, e->canvasY));
    }
    
    return 1;
}

EM_BOOL mouse_up_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    if (e->screenX != 0 && e->screenY != 0 && e->clientX != 0 && e->clientY != 0 && e->canvasX != 0 && e->canvasY != 0 && e->targetX != 0 && e->targetY != 0)
    {
        MouseHandler* mHandler = (MouseHandler*)userData;
        mHandler->SetCanvasPos(e->canvasX, e->canvasY);
        mHandler->SetState(MouseHandler::eMS_up);
        CDirector::GetDirector().TouchEnd(1, vec2(e->canvasX, e->canvasY));
    }
    
    return 1;
}

EM_BOOL mouse_move_callback(int eventType, const EmscriptenMouseEvent *e, void *userData)
{
    if (e->screenX != 0 && e->screenY != 0 && e->clientX != 0 && e->clientY != 0 && e->canvasX != 0 && e->canvasY != 0 && e->targetX != 0 && e->targetY != 0)
    {
        MouseHandler* mHandler = (MouseHandler*)userData;
        if (mHandler->GetState() == MouseHandler::eMS_down)
        {
            mHandler->SetCanvasPos(e->canvasX, e->canvasY);
            CDirector::GetDirector().TouchMove(1, vec2(e->canvasX, e->canvasY));
        }
    }
    
    return 1;
}

MouseHandler::MouseHandler()
:m_state(eMS_up)
{
    m_canvasPos = vec2();
    
    EMSCRIPTEN_RESULT ret;
    
//    ret = emscripten_set_click_callback("#canvas", this, 1, mouse_callback);
//    TEST_RESULT(emscripten_set_click_callback);
    ret = emscripten_set_mousedown_callback("#canvas", this, 1, mouse_down_callback);
    TEST_RESULT(emscripten_set_mousedown_callback);
    ret = emscripten_set_mouseup_callback("#canvas", this, 1, mouse_up_callback);
    TEST_RESULT(emscripten_set_mouseup_callback);
//    ret = emscripten_set_dblclick_callback("#canvas", this, 1, mouse_callback);
//    TEST_RESULT(emscripten_set_dblclick_callback);
    ret = emscripten_set_mousemove_callback("#canvas", this, 1, mouse_move_callback);
    TEST_RESULT(emscripten_set_mousemove_callback);
}

void MouseHandler::SetCanvasPos(float x, float y)
{
    m_canvasPos.x = x;
    m_canvasPos.y = y;
}
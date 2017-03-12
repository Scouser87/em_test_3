//
//  main.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include <iostream>
//#include "Scene.hpp"
#include <unistd.h>
#include <emscripten.h>
#include "CDirector.h"
#include "GameScene.hpp"

void MainLoop()
{
    CDirector::GetDirector().Update();
}

int main(int argc, const char * argv[]) {
    
//    Scene* scene = new Scene(1024, 768);
//    scene->Start();
    
    CDirector::GetDirector().Init(1136, 768);
    CGameScene* gs = new CGameScene;
    CDirector::GetDirector().GetRooot()->AddChild(gs);    

    emscripten_set_main_loop(MainLoop, 60, 0);
    
    return 0;
}

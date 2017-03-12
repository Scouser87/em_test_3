//
//  main.cpp
//  em_test_3
//
//  Created by Sergei Makarov on 09.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#include <iostream>
#include "Scene.hpp"

int main(int argc, const char * argv[]) {
    
    Scene* scene = new Scene(1024, 768);
    scene->Start();
    
    return 0;
}

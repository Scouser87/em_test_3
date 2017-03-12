//
//  CImage.hpp
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef CImage_hpp
#define CImage_hpp

#include <string>

class CImage
{
public:
    CImage(std::string path);
    
    void BindTexture();
    
    const float& GetMaxS() const { return maxS; }
    const float& GetMaxT() const { return maxT; }
    
    const float& GetWidth() const { return m_width; }
    const float& GetHeight() const { return m_height; }
    
private:
    int m_id;
    int m_width;
    int m_height;
        
    float minS;
    float minT;
    float maxS;
    float maxT;
};

#endif /* CImage_hpp */

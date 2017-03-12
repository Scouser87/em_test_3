//
//  sColor.h
//  em_test_3
//
//  Created by Sergei Makarov on 12.03.17.
//  Copyright © 2017 Sergei Makarov. All rights reserved.
//

#ifndef sColor_h
#define sColor_h

#include <string>

inline float clamp(float d,float min,float max)
{
    if(d>max)d=max;
    if(d<min)d=min;
    return d;
}

struct sColor
{
    union
    {
        struct{float n[4];};
        struct{float sRed, sGreen, sBlue, sAlpha;};
    };
    
    sColor()								{sRed=1.0;sGreen=1.0;sBlue=1.0;sAlpha=1.0;}
    sColor(float r,float g,float b,float a = 1.0f)	{sRed=r;sGreen=g;sBlue=b;sAlpha=a;}
    static  sColor IntColor(int r,int g,int b,int a = 255){sColor color; color.sRed=(float)r/255.0f; color.sGreen=(float)g/255.0f; color.sBlue=(float)b/255.0f; color.sAlpha=(float)a/255.0f; return color;}
    static  sColor HexColor(std::string hex);
    
    sColor& operator+=(const sColor& c)
    {
        sRed += c.sRed; sGreen += c.sGreen;	sBlue += c.sBlue; sAlpha += c.sAlpha;
        sRed=clamp(sRed,-1,1); sGreen=clamp(sGreen,-1,1);	sBlue=clamp(sBlue,-1,1); sAlpha=clamp(sAlpha,-1,1);
        return *this;
    }
    sColor& operator-=(const sColor& c)
    {
        sRed -= c.sRed; sGreen -= c.sGreen;	sBlue -= c.sBlue; sAlpha -= c.sAlpha;
        sRed = clamp(sRed,-1,1); sGreen = clamp(sGreen,-1,1); sBlue = clamp(sBlue,-1,1); sAlpha=clamp(sAlpha,-1,1);
        return *this;
    }
    sColor operator+(const sColor& c)
    {
        sColor color;
        color = *this;
        color.sRed += c.sRed; color.sGreen += c.sGreen;	color.sBlue += c.sBlue;	color.sAlpha += c.sAlpha;
        color.sRed = clamp(color.sRed,-1,1);
        color.sGreen = clamp(color.sGreen,-1,1);
        color.sBlue = clamp(color.sBlue,-1,1);
        color.sAlpha = clamp(color.sAlpha,-1,1);
        return color;
    }
    sColor& operator-()
    {
        sRed = 1 - sRed;
        sGreen = 1 - sGreen;
        sBlue = 1 - sBlue;
        sAlpha = 1 - sAlpha;
        return *this;
    }
    sColor operator-(const sColor& c)
    {
        sColor color;
        color = *this;
        color.sRed -= c.sRed; color.sGreen -= c.sGreen;	color.sBlue -= c.sBlue; color.sAlpha -= c.sAlpha;
        color.sRed = clamp(color.sRed,-1,1);
        color.sGreen = clamp(color.sGreen,-1,1);
        color.sBlue = clamp(color.sBlue,-1,1);
        color.sAlpha = clamp(color.sAlpha,-1,1);
        return color;
    }
    sColor operator*(const float& c)
    {
        sColor color;
        color=*this;
        
        color.sRed*=c;color.sGreen*=c;color.sBlue*=c;color.sAlpha*=c;
        
        color.sRed=clamp(color.sRed,-1,1);
        color.sGreen=clamp(color.sGreen,-1,1);
        color.sBlue=clamp(color.sBlue,-1,1);
        color.sAlpha = clamp(color.sAlpha,-1,1);
        
        return color;
    }
    sColor operator/(const float& c)
    {
        sColor color;
        color=*this;
        
        color.sRed/=c;color.sGreen/=c;color.sBlue/=c;color.sAlpha/=c;
        
        color.sRed=clamp(color.sRed,-1,1);
        color.sGreen=clamp(color.sGreen,-1,1);
        color.sBlue=clamp(color.sBlue,-1,1);
        color.sAlpha = clamp(color.sAlpha,-1,1);
        
        return color;
    }
    bool operator==(const sColor& c) const
    {
        return (sRed == c.sRed && sGreen == c.sGreen && sBlue == c.sBlue && sAlpha == c.sAlpha);
    }
    bool operator!=(const sColor& c) const
    {
        return !(sRed == c.sRed && sGreen == c.sGreen && sBlue == c.sBlue && sAlpha == c.sAlpha);
    }
    bool IsDefault() const;
    
    float& operator[](int i);
    const float& operator[](int i) const;
    
    void SetAlpha(const float& a) {sAlpha=a;}
    void SetWhite(){sRed=1.0;sGreen=1.0;sBlue=1.0;sAlpha=1.0;}
    
    static sColor White;
    static sColor Black;
    static sColor Red;
    static sColor Green;
    static sColor Blue;
};

#endif /* sColor_h */

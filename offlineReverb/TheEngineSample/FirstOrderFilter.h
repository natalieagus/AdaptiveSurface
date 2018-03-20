//
//  FirstOrderFilter.h
//  ViolinModel
//
//  Created by Hans on 1/7/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#ifndef __ViolinModel__FirstOrderFilter__
#define __ViolinModel__FirstOrderFilter__

#include <stdio.h>

class FirstOrderFilter{
private:
    float za, zb;
    float a1, b0, b1;
    
public:
    FirstOrderFilter();
    void setHighShelf(float fc, float gain);
    void setLowPass(float fc);
    void setLowShelf(float fc, float gain);
    float process(float sample);
    void setAngle(float theta, float fc, bool right);
};

#endif /* defined(__ViolinModel__FirstOrderFilter__) */

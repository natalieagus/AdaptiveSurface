//
//  BiQuadFilter.h
//  ViolinModel
//
//  Created by Hans on 1/7/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#ifndef __ViolinModel__BiQuadFilter__
#define __ViolinModel__BiQuadFilter__

#include <stdio.h>

class BiQuadFilter {
private:
    float za1, za2, zb1, zb2;
    float a1, a2, b0, b1, b2;
    
public:
    float process(float sample);
    BiQuadFilter(float a1, float a2, float b0, float b1, float b2);
    BiQuadFilter();
    void setHighShelf(float fc, float gain);
    void setBandPass(float fc, float bw);
    void setLowPass(float fc);
    void setHighPass(float fc);
};

#endif /* defined(__ViolinModel__BiQuadFilter__) */

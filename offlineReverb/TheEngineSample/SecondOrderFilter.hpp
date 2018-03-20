//
//  SecondOrderFilter.hpp
//  TheEngineSample
//
//  Created by Natalie Agus on 2/11/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#ifndef SecondOrderFilter_hpp
#define SecondOrderFilter_hpp

#include <stdio.h>

class SecondOrderFilter{

public:
    SecondOrderFilter();
    void setNotchFilter(float notch, float fs, float bw);
    float process(float input);
    void setResonatorBlock(float G, float fs, float fc);
    void setReflectionBlock(float G, float fs, float fc); //Magnitude in dB, sampling frequency, and centre frequency
    

private:
    float b2,b1,b0; //numerator
    float a2,a1,a0; //denominator
    float x1, x2;
    float y1, y2;
    float fB = 5.0f; //fixed bandwith of 5.0 kHz
};

#endif /* SecondOrderFilter_hpp */

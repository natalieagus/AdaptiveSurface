//
//  SecondOrderFilter.cpp
//  TheEngineSample
//
//  Created by Natalie Agus on 2/11/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#include "SecondOrderFilter.hpp"
#include <math.h>

SecondOrderFilter::SecondOrderFilter(){
    a1 = a2 = b0 = b1 = b2 = x1 = x2 = y1 = y2 = 0.0f;
    a0 = 1.0f;
}

//cascade notch filters to get multiple notches
//input is notch frequency and sampling rate
void SecondOrderFilter::setNotchFilter(float notch, float fs, float bw){
    float r = 1.0f - (bw/fs * M_PI);
    float theta = notch / fs * 2* M_PI;
    b0 = 1.0f;
    b1 = -2.0f * cos(theta);
    b2 = 1.0f;
    a0 = 1.0f;
    a1 = -2.0f*r*cos(theta);
    a2 = pow(r, 2.0f);
}

//Parallel
//G = 15, fs = 44100, fc = 4khz
//G = 10, fs = 44100, fc = 13khz
void SecondOrderFilter::setResonatorBlock(float G, float fs, float fc){
    float h = 1.0f/ (1.0f + tan(M_PI * fB / fs));
    float d = -cos(2 * M_PI * fc/fs);
    float v_o = powf(10.0f, G/20.f);
    a0 = 1.0f;
    a1 = 2 * d * h;
    a2 = 2 * h -1;
    b0 = v_o * (1-h);
    b1 = 0.0f;
    b2 = -v_o * (1-h);
}

//Cascade
//G = -10, fs = 44100, fc = 7khz
//G =-35, fs = 44100, fc = 9khz
//G = -40, fs = 44100, fc = 11khz
void SecondOrderFilter::setReflectionBlock(float G, float fs, float fc){
    float v_o = powf(10.0f, G/20.f);
    float h_o = v_o - 1;
    float d = -cos(2 * M_PI * fc/fs);
    float k = ((tan(M_PI*fB/fs) - v_o) / (tan(M_PI*fB/fs) + v_o));
    b0 = 1 + (1+k)*h_o/2;
    b1 = d*(1-k);
    b2 = -k - ((1-k)*h_o/2);
    a0 = 1.0f;
    a1 = d*(1-k);
    a2 = -k;
}

float SecondOrderFilter::process(float input){
    float output = (b0 * input + b1 * x1 + b2 * x2 - (a1 * y1 + a2 * y2))/a0;
    //update variables
    x2 = x1;
    x1 = input;
    y2 = y1;
    y1 = output;
    return output;
}

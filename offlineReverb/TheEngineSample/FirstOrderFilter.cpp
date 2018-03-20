//
//  FirstOrderFilter.cpp
//  ViolinModel
//
//  Created by Hans on 1/7/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#include <math.h>
#include "FirstOrderFilter.h"
#include "Constants.h"
#include "Math.h"



FirstOrderFilter::FirstOrderFilter(){
    a1 = b0 = b1 = za = zb = 0.0f;
}

float FirstOrderFilter::process(float sample){
    float out = sample*b0 + zb*b1 - za*a1;
    zb = sample;
    za = out;
    return out;
}

//Theta input has to be between 0 to 180 and 0 to -180
void FirstOrderFilter::setAngle(float theta, float fc, bool right){
//    printf("theta %f \n", theta);
    
    float theta0 = 150 ;
    float alfa_min = 0.1f ;
    float c = 334.0f; // speed of sound
    float a = 0.084f; // radius of head
    float w0 = c/a;
    float alfa = (1.0f + alfa_min/2.0f) + (1.0f - alfa_min/2.0f)* cos((theta / theta0)* M_PI);
    
    b0 = (alfa+w0/fc)/(1.0+w0/fc);
    b1 = (-alfa+w0/fc)/(1.0+w0/fc);
    a1 = -(1.0-w0/fc)/(1.0+w0/fc);
}

// See Digital Filters for Everyone by Rusty Allred 2.3.10
void FirstOrderFilter::setHighShelf(float fc, float gain){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float one_over_denominator;
    
    if (gain > 1.0f) {
        one_over_denominator = 1.0f / (gamma + 1.0f);
        b0 = (gamma + gain) * one_over_denominator;
        b1 = (gamma - gain) * one_over_denominator;
        a1 = (gamma - 1.0f) * one_over_denominator;
    } else { // gain <= 1
        one_over_denominator = 1.0f / (gain*gamma + 1.0f);
        b0 = gain*(gamma + 1.0f) * one_over_denominator;
        b1 = gain*(gamma - 1.0f) * one_over_denominator;
        a1 = (gain*gamma - 1.0f) * one_over_denominator;
    }
}

void FirstOrderFilter::setLowShelf(float fc, float gain){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float one_over_denominator;
    
    if (gain > 1.0f) {
        one_over_denominator = 1.0f / (gamma + 1.0f);
        b0 = (gamma * gain + 1.0f) * one_over_denominator;
        b1 = (gamma * gain - 1.0f) * one_over_denominator;
        a1 = (gamma - 1.0f) * one_over_denominator;
    } else { // gain <= 1
        one_over_denominator = 1.0f / (gain + gamma);
        b0 = gain*(gamma + 1.0f) * one_over_denominator;
        b1 = gain*(gamma - 1.0f) * one_over_denominator;
        a1 = (gamma - gain) * one_over_denominator;
    }
}

void FirstOrderFilter::setLowPass(float fc){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float one_over_denominator = 1.0f / (gamma + 1.0f);
    
    b0 = gamma * one_over_denominator;
    b1 = b0;
    
    a1 = (gamma - 1.0f) * one_over_denominator;
}

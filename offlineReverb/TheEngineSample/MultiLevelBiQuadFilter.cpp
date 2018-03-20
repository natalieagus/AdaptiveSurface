//
//  MultiLevelBiQuadFilter.cpp
//  TheEngineSample
//
//  Created by Hans on 4/9/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#include "MultiLevelBiQuadFilter.h"
#include "Constants.h"
#include "assert.h"



void MultiLevelBiQuadFilter::processBuffer(float* input, float* output, vDSP_Length numFrames){
    if (setupChangeQueued) doSetup();
    vDSP_biquad(setup, pastStateData, input, 1, output, 1, numFrames);
}

void MultiLevelBiQuadFilter::doSetup(){


    setup = vDSP_biquad_CreateSetup(coefficients, NUMLEVELS);
    setupChangeQueued = false;
}

inline void setBypass(double* b0, double* b1, double* b2, double* a1, double* a2){
    *b0 = 1.0;
    *b1 = *b2 = *a1 = *a2;
}

void MultiLevelBiQuadFilter::setBell(double fc, double bw, double gain, UInt16 number){
    // we currently allow only three of these, numbered 1, 2, and 3
    assert(number <= 3 && number > 0);
    
    // the bells are on the 3rd, 4th, and 5th levels (indices 2, 3, 4)
    double* b0 = &coefficients[5*(number+1)];
    double* b1 = b0 + 1;
    double* b2 = b1 + 1;
    double* a1 = b2 + 1;
    double* a2 = a1 + 1;
    
    // if gain is close to 1.0, bypass the filter
    if (std::abs(log(gain)) < 0.01)
        setBypass(b0, b1, b2, a1, a2);
    
    // if the gain is nontrivial
    else {
        // see 2.3.8 in Digital Filters are for Everyone, 2nd ed. by Rusty Allred
        double alpha =  tan( (M_PI * bw)   / SAMPLE_RATE_D);
        double beta  = -cos( (M_2_PI * fc) / SAMPLE_RATE_D);
        double oneOverD;
        
        if (gain < 1.0) {
            oneOverD = 1.0 / (alpha + gain);
            // feed-forward coefficients
            *b0 = (gain + alpha*gain) * oneOverD;
            *b1 = 2.0 * beta * gain * oneOverD;
            *b2 = (gain - alpha*gain) * oneOverD;
            
            // recursive coefficients
            *a1 = 2.0 * beta * gain * oneOverD;
            *a2 = (gain - alpha) * oneOverD;
        } else { // gain >= 1
            oneOverD = 1.0 / (alpha + 1.0);
            // feed-forward coefficients
            *b0 = (1.0 + alpha*gain) * oneOverD;
            *b1 = 2.0 * beta * oneOverD;
            *b2 = (1.0 - alpha*gain) * oneOverD;
            
            // recursive coefficients
            *a1 = 2.0 * beta * oneOverD;
            *a2 = (1.0 - alpha) * oneOverD;
        }
    }
    
    setupChangeQueued = true;
}

void MultiLevelBiQuadFilter::setLowPass(double fc){
    // the lowpass filter is on the second level
    int level = 1;
    double* b0 = &coefficients[5*level];
    double* b1 = b0 + 1;
    double* b2 = b1 + 1;
    double* a1 = b2 + 1;
    double* a2 = a1 + 1;
    
    // if fc is greater than 95% of the nyquyst frequency, bypass the filter
    if (fc > 0.95*0.5*SAMPLE_RATE_D)
        setBypass(b0, b1, b2, a1, a2);
    
    
    // else, the cutoff frequency is non-trivial
    else {
        double gamma = tan(M_PI * fc / SAMPLE_RATE_D);
        double gamma_2 = gamma * gamma;
        double gamma_x_sqrt_2 = gamma * M_SQRT2;
        double one_over_denominator = 1.0 / (gamma_2 + gamma_x_sqrt_2 + 1.0);
        
        *b0 = gamma_2 * one_over_denominator;
        *b1 = 2.0 * *b0;
        *b2 = *b0;
        
        *a1 = 2.0 * (gamma_2 - 1.0) * one_over_denominator;
        *a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0) * one_over_denominator;
    }
    
    setupChangeQueued = true;
}

void MultiLevelBiQuadFilter::set2ndOrderHighPass(double fc){
    // the highpass filter is on the first level
    int level = 0;
    double* b0 = &coefficients[5*level];
    double* b1 = b0 + 1;
    double* b2 = b1 + 1;
    double* a1 = b2 + 1;
    double* a2 = a1 + 1;
    
    // if the cutoff frequency is less than 10 hz, bypass the filter
    if (fc < 10.0)
        setBypass(b0, b1, b2, a1, a2);
    
    // else, for useful settings of fc, setup the filter
    else {
        double gamma = tan(M_PI * fc / SAMPLE_RATE_D);
        double gamma_2 = gamma * gamma;
        double gamma_x_sqrt_2 = gamma * M_SQRT2;
        double one_over_denominator = 1.0 / (gamma_2 + gamma_x_sqrt_2 + 1.0);
        
        *b0 = 1.0 * one_over_denominator;
        *b1 = -2.0 * one_over_denominator;
        *b2 = *b0;
        
        *a1 = 2.0 * (gamma_2 - 1.0) * one_over_denominator;
        *a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0) * one_over_denominator;
    }
    
    setupChangeQueued = true;
}

void MultiLevelBiQuadFilter::set1stOrderHighPass(double fc){
    // the highpass filter is on the first level
    int level = 0;
    double* b0 = &coefficients[5*level];
    double* b1 = b0 + 1;
    double* b2 = b1 + 1;
    double* a1 = b2 + 1;
    double* a2 = a1 + 1;
    
    // if the cutoff frequency is less than 10 hz, bypass the filter
    if (fc < 10.0)
        setBypass(b0, b1, b2, a1, a2);
    
    // else, for useful settings of fc, setup the filter
    else {
        double gamma = tan(M_PI * fc / SAMPLE_RATE_D);
        double one_over_denominator = 1.0 / (gamma + 1.0);
        
        *b0 = 1.0 * one_over_denominator;
        *b1 = -1.0 * one_over_denominator;
        *b2 = 0.0;
        
        *a1 = (gamma - 1.0) * one_over_denominator;
        *a2 = 0.0;
    }
    
    setupChangeQueued = true;
}

MultiLevelBiQuadFilter::MultiLevelBiQuadFilter(){
    // set all the filter levels to passthrough (b0 = 1; b1=b2=a1=a2=0;)
    vDSP_vclrD(coefficients, 1, NUMLEVELS*5);
    for (int i = 0; i < NUMLEVELS; i++) coefficients[i*5] = 1.0;
    
    // initialize the delays to zero
    vDSP_vclr(pastStateData, 1, NUMLEVELS*4);
    
    doSetup();
}

MultiLevelBiQuadFilter::~MultiLevelBiQuadFilter(){
//    vDSP_biquad_DestroySetup(setup);
}

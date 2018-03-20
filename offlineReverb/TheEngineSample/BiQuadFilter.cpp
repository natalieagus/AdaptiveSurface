//
//  BiQuadFilter.cpp
//  ViolinModel
//
//  Created by Hans on 1/7/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#include <math.h>
#include "BiQuadFilter.h"
#include "Constants.h"

BiQuadFilter::BiQuadFilter(float a1, float a2, float b0, float b1, float b2){
    this->a1 = a1;
    this->a2 = a2;
    this->b0 = b0;
    this->b1 = b1;
    this->b2 = b2;
    za1 = za2 = zb1 = zb2 = 0.0f;
}

BiQuadFilter::BiQuadFilter(){
    za1 = za2 = zb1 = zb2 = a1 = a2 = b0 = b1 = b2 = 0.0f;
}


// See
// https://ccrma.stanford.edu/~jos/fp/Direct_Form_I.html
//
// For formulae to calculate filter coefficients, see Digital Filters for Everyone by Rusty Alred
float BiQuadFilter::process(float sample){
    float out = sample*b0 + zb1*b1 + zb2*b2 - za1*a1 - za2 *a2;
    zb2 = zb1;
    zb1 = sample;
    za2 = za1;
    za1 = out;
    return out;
}

// see 2.3.1.3 of Digital Filters for Everyone by Rusty Allred
void BiQuadFilter::setBandPass(float fc, float bw){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float gamma_2 = gamma * gamma;
    float gamma_2_plus_one_times_fc = (gamma_2 + 1.0f) * fc;
    float gamma_times_bw = gamma * bw;
    float one_over_denominator = 1.0f / (gamma_2_plus_one_times_fc + gamma_times_bw);
    
    b0 = gamma_times_bw * one_over_denominator;
    b1 = 0.0f;
    b2 = -b0;
    
    a1 = 2.0f * fc * (gamma_2 - 1.0f) * one_over_denominator;
    a2 = (gamma_2_plus_one_times_fc - gamma_times_bw) * one_over_denominator;
}

// see 2.3.10 of Digital Filters for Everyone by Rusty Allred
void BiQuadFilter::setHighShelf(float fc, float gain){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float gamma_2 = gamma*gamma;
    float sqrt_gain = sqrtf(gain);
    
    // conditionally set G
    float G;
    if (gain > 2.0f) G = gain * M_SQRT2 * 0.5f;
    else {
        if (gain >= 0.5f) G = sqrt_gain;
        else G = gain * M_SQRT2;
    }
    float G_2 = G*G;
    
    // compute reuseable variables
    float g_d = powf((G_2 - 1.0f)/(gain*gain - G_2), 0.25f);
    float g_d_2 = g_d*g_d;
    float g_n = g_d * sqrt_gain;
    float g_n_2 = g_n * g_n;
    float sqrt_2_g_d_gamma = M_SQRT2 * g_d * gamma;
    float sqrt_2_g_n_gamma = M_SQRT2 * g_n * gamma;
    float gamma_2_plus_g_d_2 = gamma_2 + g_d_2;
    float gamma_2_plus_g_n_2 = gamma_2 + g_n_2;
    
    float one_over_denominator = 1.0f / (gamma_2_plus_g_d_2 + sqrt_2_g_d_gamma);
    
    b0 = (gamma_2_plus_g_n_2 + sqrt_2_g_n_gamma) * one_over_denominator;
    b1 = 2.0f * (gamma_2 - g_n_2) * one_over_denominator;
    b2 = (gamma_2_plus_g_n_2 - sqrt_2_g_n_gamma) * one_over_denominator;
    
    a1 = 2.0f * (gamma_2 - g_d_2) * one_over_denominator;
    a2 = (gamma_2_plus_g_d_2 - sqrt_2_g_d_gamma)*one_over_denominator;
}

void BiQuadFilter::setLowPass(float fc){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float gamma_2 = gamma * gamma;
    float gamma_x_sqrt_2 = gamma * M_SQRT2;
    float one_over_denominator = 1.0f / (gamma_2 + gamma_x_sqrt_2 + 1.0f);
    
    b0 = gamma_2 * one_over_denominator;
    b1 = 2.0f * b0;
    b2 = b0;
    
    a1 = 2.0f * (gamma_2 - 1.0f) * one_over_denominator;
    a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0f) * one_over_denominator;
}

void BiQuadFilter::setHighPass(float fc){
    float gamma = tanf(M_PI * fc / SAMPLE_RATE_F);
    float gamma_2 = gamma * gamma;
    float gamma_x_sqrt_2 = gamma * M_SQRT2;
    float one_over_denominator = 1.0f / (gamma_2 + gamma_x_sqrt_2 + 1.0f);
    
    b0 = 1.0f * one_over_denominator;
    b1 = -2.0f * one_over_denominator;
    b2 = b0;
    
    a1 = 2.0f * (gamma_2 - 1.0f) * one_over_denominator;
    a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0f) * one_over_denominator;
}

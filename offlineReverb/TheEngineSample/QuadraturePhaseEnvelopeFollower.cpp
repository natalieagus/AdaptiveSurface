//
//  QuadraturePhaseEnvelopeFollower.cpp
//  TheEngineSample
//
//  Created by Hans on 14/9/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#include "QuadraturePhaseEnvelopeFollower.h"

QuadraturePhaseEnvelopeFollower::QuadraturePhaseEnvelopeFollower(){
    setHighpassCoefficients();
    setLowpassCoefficients();
    bandpassFilter = vDSP_biquad_CreateSetup(bandpassCoefficients,2);
    allpassCascade1 = vDSP_biquad_CreateSetup(ap1coefficients, ALLPASS_CASCADE_LENGTH);
    allpassCascade2 = vDSP_biquad_CreateSetup(ap2coefficients, ALLPASS_CASCADE_LENGTH);
}

QuadraturePhaseEnvelopeFollower::~QuadraturePhaseEnvelopeFollower(){
    vDSP_biquad_DestroySetup(allpassCascade1);
    vDSP_biquad_DestroySetup(allpassCascade2);
}

void QuadraturePhaseEnvelopeFollower::setHighpassCoefficients(){
    double fc_rad = 0.005*M_PI;
    
    // the highpass filter is on the first level
    int level = 0;
    double* b0 = &bandpassCoefficients[5*level];
    double* b1 = b0 + 1;
    double* b2 = b1 + 1;
    double* a1 = b2 + 1;
    double* a2 = a1 + 1;

    double gamma = tan(fc_rad);
    double gamma_2 = gamma * gamma;
    double gamma_x_sqrt_2 = gamma * M_SQRT2;
    double one_over_denominator = 1.0 / (gamma_2 + gamma_x_sqrt_2 + 1.0);

    *b0 = 1.0 * one_over_denominator;
    *b1 = -2.0 * one_over_denominator;
    *b2 = *b0;

    *a1 = 2.0 * (gamma_2 - 1.0) * one_over_denominator;
    *a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0) * one_over_denominator;
}

void QuadraturePhaseEnvelopeFollower::setLowpassCoefficients(){
    double fc_rad = (1.0-0.005)*M_PI;
    
    // the lowpass filter is on the second level
    int level = 1;
    double* b0 = &bandpassCoefficients[5*level];
    double* b1 = b0 + 1;
    double* b2 = b1 + 1;
    double* a1 = b2 + 1;
    double* a2 = a1 + 1;
    
    double gamma = tan(fc_rad);
    double gamma_2 = gamma * gamma;
    double gamma_x_sqrt_2 = gamma * M_SQRT2;
    double one_over_denominator = 1.0 / (gamma_2 + gamma_x_sqrt_2 + 1.0);
    
    *b0 = gamma_2 * one_over_denominator;
    *b1 = 2.0 * *b0;
    *b2 = *b0;
    
    *a1 = 2.0 * (gamma_2 - 1.0) * one_over_denominator;
    *a2 = (gamma_2 - gamma_x_sqrt_2 + 1.0) * one_over_denominator;
}

void QuadraturePhaseEnvelopeFollower::getEnvelopeForBuffer(float *input, float* output, UInt16 numFrames){
    // bandpass filter and drop the sample rate by a factor of 4
    vDSP_biquad(bandpassFilter, bpdelays, input, 4, buffer, 1, numFrames/4);
    
    // run the first APF on the buffer and write the output to the envelope
    vDSP_biquad(allpassCascade1, ap1delays, buffer, 1, envelope1, 1, numFrames/4);
    
    // run the second APF on the buffer and write the output to the envelope
    vDSP_biquad(allpassCascade2, ap2delays, buffer, 1, envelope2, 1, numFrames/4);
    
    // buffer = sqrt(env1^2 + env2^2)
   // there is a VDSP command for this; it's the pythagorean one.'
}
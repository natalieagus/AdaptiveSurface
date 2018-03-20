//
//  QuadraturePhaseEnvelopeFollower.h
//  TheEngineSample
//
//  Created by Hans on 14/9/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#ifndef __TheEngineSample__QuadraturePhaseEnvelopeFollower__
#define __TheEngineSample__QuadraturePhaseEnvelopeFollower__

#define ALLPASS_CASCADE_LENGTH 6

#import <Accelerate/Accelerate.h>

class QuadraturePhaseEnvelopeFollower {
    QuadraturePhaseEnvelopeFollower();
    ~QuadraturePhaseEnvelopeFollower();
    void getEnvelopeForBuffer(float* input, float* output, UInt16 numFrames);
private:
    float buffer [512];
    float envelope1 [512];
    float envelope2 [512];
    vDSP_biquad_Setup allpassCascade1;
    vDSP_biquad_Setup allpassCascade2;
    vDSP_biquad_Setup bandpassFilter;
    
    // these coefficients are computed in mathematica in the file called QuadratureAllpassNetworks
    // they approximate quadrature on frequencies from .01*M_PI to 0.99*M_PI
    double ap1coefficients [ALLPASS_CASCADE_LENGTH*5] =
        {0.955448, -1.95523, 1, -1.95523, 0.955448,
        0.610377, -1.57349, 1, -1.57349, 0.610377,
        0.0639635, -0.53388, 1, -0.53388, 0.0639635,
        0.0215215, 0.305505, 1, 0.305505, 0.0215215,
        0.436638, 1.34709, 1, 1.34709, 0.436638,
        0.911355, 1.91001, 1, 1.91001, 0.911355};
    double ap2coefficients [ALLPASS_CASCADE_LENGTH*5] =
        {0.955448, 1.95523, 1, 1.95523, 0.955448,
         0.610377, 1.57349, 1, 1.57349, 0.610377,
        0.0639635, 0.53388, 1, 0.53388, 0.0639635,
        0.0215215, -0.305505, 1, -0.305505, 0.0215215,
        0.436638, -1.34709, 1,-1.34709, 0.436638,
        0.911355, -1.91001, 1, -1.91001, 0.911355};
    
    double bandpassCoefficients [5*2];
    
    float ap1delays [ALLPASS_CASCADE_LENGTH*4];
    float ap2delays [ALLPASS_CASCADE_LENGTH*4];
    float bpdelays [2*4];
    
    void setHighpassCoefficients();
    void setLowpassCoefficients();
};

#endif /* defined(__TheEngineSample__QuadraturePhaseEnvelopeFollower__) */

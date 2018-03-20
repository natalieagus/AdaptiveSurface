//
//  MultiLevelBiQuadFilter.h
//  TheEngineSample
//
//  Created by Hans on 4/9/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#ifndef __TheEngineSample__MultiLevelBiQuadFilter__
#define __TheEngineSample__MultiLevelBiQuadFilter__

#import <Accelerate/Accelerate.h>
#define NUMLEVELS 5

class MultiLevelBiQuadFilter {
public:
    MultiLevelBiQuadFilter();
    ~MultiLevelBiQuadFilter();
    void set2ndOrderHighPass(double fc);
    void set1stOrderHighPass(double fc);
    void setLowPass(double fc);
    void setBell(double fc, double bw, double gain, UInt16 number);
    void processBuffer(float* input, float* output, vDSP_Length numFrames);
    
private:
    vDSP_biquad_Setup setup ;
    double coefficients [NUMLEVELS*5];
    float pastStateData [NUMLEVELS*4];
    void doSetup();
    bool setupChangeQueued;
};




#endif /* defined(__TheEngineSample__MultiLevelBiQuadFilter__) */

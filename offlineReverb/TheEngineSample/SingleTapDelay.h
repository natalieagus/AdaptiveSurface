//
//  SingleTapDelay.h
//  TheEngineSample
//
//  Created by Hans on 28/9/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#ifndef SingleTapDelay_h
#define SingleTapDelay_h

#include <stdio.h>

#include "Constants.h"

class SingleTapDelay {
public:
    SingleTapDelay();
    void setTimeSafe(float seconds);
    void processBuffer(float* input, float* output, size_t numFrames);
    float process(float input);
    float time;
    
    
private:
    void updateLength();
    float delayLineBuffer [MAX_DELAY_LINE];
    float frameBuffer [MAX_BUFFER_LENGTH];
    size_t rwIdx;
    size_t endMarker;
    size_t nextEndMarker;
    size_t lengthInSamples;
    bool needsUpdate;
    bool zeroDelay;
};

#endif /* SingleTapDelay_h */

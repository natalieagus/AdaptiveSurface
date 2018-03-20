//
//  SingleTapDelay.cpp
//  TheEngineSample
//
//  Created by Hans on 28/9/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#include "SingleTapDelay.h"


#include <Accelerate/Accelerate.h>
#include "Constants.h"
#include "assert.h"

SingleTapDelay::SingleTapDelay(){
    rwIdx = 0;
    setTimeSafe(0.1);
    needsUpdate = true;
    memset(delayLineBuffer, 0, MAX_DELAY_LINE*sizeof(float));
}

// process one sample at a time
// use processBuffer() if possible, to improve efficiency
float SingleTapDelay::process(float input){
    
    if (zeroDelay){
       // printf("zero delay!");
        return input;
    }
    
    if (needsUpdate)
        updateLength();
    
    float audioOutput;
    
    // copy from delay line to temp buffer
    audioOutput = delayLineBuffer[rwIdx];
    
    // write input into to the delayLine
    delayLineBuffer[rwIdx] = input;
    
    // advance the index and wrap around to the beginning if necessary
    rwIdx ++;
    if (rwIdx == endMarker) rwIdx = 0;
    
    return audioOutput;
}

// works in place
void SingleTapDelay::processBuffer(float *input, float *output, size_t numFrames){
    assert(numFrames <= MAX_BUFFER_LENGTH);
    
    // bypass for zero length buffer
    if (lengthInSamples == 0) {
        memcpy(output, input, sizeof(float)*numFrames);
        return;
    }
    
    size_t distanceToEndMarker, distanceToEndOfInput;
    size_t framesInNextCopy;
    size_t framesCopied = 0;
    
    if (needsUpdate) updateLength();
    
    while (framesCopied < numFrames){
        distanceToEndMarker = endMarker - rwIdx;
        distanceToEndOfInput = numFrames - framesCopied;
        framesInNextCopy = distanceToEndOfInput < distanceToEndMarker ? distanceToEndOfInput:distanceToEndMarker; // MIN
        
        // copy from delayLineBuffer to frameBuffer
        memcpy(frameBuffer, &delayLineBuffer[rwIdx], sizeof(float)*framesInNextCopy);
        
        // copy from input to delayLineBuffer
        memcpy(&delayLineBuffer[rwIdx], &input[framesCopied], sizeof(float)*framesInNextCopy);
        
        // copy frameBuffer to output
        memcpy(&output[framesCopied], &frameBuffer, sizeof(float)*framesInNextCopy);
         
        framesCopied += framesInNextCopy;
        
        // advance the index and wrap around to the beginning if necessary
        rwIdx += framesInNextCopy;
        if (rwIdx == endMarker) rwIdx = 0;
    }
}

// safely request a delay time update at the next available opportunity
void SingleTapDelay::setTimeSafe(float seconds){
 //  printf("seconds: %f \n", seconds);
    this->time = seconds;
    if (seconds < 0.0f){
        seconds = 0.0f;
    }
 //   printf("seconds: %f \n", seconds);
    if (fabs(0.0f - seconds) < 0.0000001f){
       // printf("Zero delay is true \n");
        zeroDelay = true;
    }
    nextEndMarker = seconds*SAMPLE_RATE_F;
    lengthInSamples = nextEndMarker;
    ///printf("MAX DELAY LINE: %d \n", MAX_DELAY_LINE);
   //printf("*********\n");
    assert(nextEndMarker <= MAX_DELAY_LINE);
    needsUpdate = true;
}

inline void SingleTapDelay::updateLength(){
    endMarker = nextEndMarker;
    if (rwIdx >= endMarker) rwIdx = 0;
    memset(delayLineBuffer, 0, sizeof(float)*MAX_DELAY_LINE);
    needsUpdate = false;
}
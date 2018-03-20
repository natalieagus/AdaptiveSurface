//
//  DoubleBufferedReverb.cpp
//  TheEngineSample
//
//  Created by Natalie Agus on 27/10/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#include "DoubleBufferedReverb.h"

using namespace std;

DoubleBufferedReverb::DoubleBufferedReverb(void)
{
    // set reverb2 to be the back buffer reverb
    backReverb = &reverb2;
    
    // set the gain on reverb 1 to 1.0f;
    r1_gainTarget = 1.0f;
    
    for (int i = 0; i < DB_REVERB_BUFFER_SIZE; i++){
        r1_gain[i] = 1.0f;
    }
}

size_t min(size_t a, size_t b){ return a < b ? a : b; }

void DoubleBufferedReverb::processIFretlessBuffer(float* input, size_t numFrames, float * outputL, float* outputR){
   
    size_t framesToProcess = numFrames;
    while (framesToProcess > 0) {

        size_t framesInThisIteration = min(framesToProcess,DB_REVERB_BUFFER_SIZE);
        
        // process the reverbs into the buffers
        size_t outputIndex = numFrames - framesToProcess;
        reverb1.processIFretlessBuffer(&input[outputIndex], framesInThisIteration,  l_buffer_1,r_buffer_1);
        reverb2.processIFretlessBuffer(&input[outputIndex], framesInThisIteration, l_buffer_2, r_buffer_2);
//        
        // compute the gains for reverb1
        float r1gainAtFrameStart = r1_gain[framesInThisIteration-1];
        float r1gainAtFrameEnd = r1gainAtFrameStart + 0.5*(r1_gainTarget - r1gainAtFrameStart);
        //printf("gain 1 at frame start: %f, gain at frame end: %f\n", r1gainAtFrameStart, r1gainAtFrameEnd);
        vDSP_vgen(&r1gainAtFrameStart, &r1gainAtFrameEnd, r1_gain, 1, framesInThisIteration);
        
        // compute the gains for reverb2
        float r2gainAtFrameStart = sqrtf(1.0f - (r1gainAtFrameStart*r1gainAtFrameStart));
        float r2gainAtFrameEnd = sqrt(1.0f - (r1gainAtFrameEnd*r1gainAtFrameEnd));
       // printf("gain 2 at frame start: %f, gain at frame end: %f\n", r2gainAtFrameStart, r2gainAtFrameEnd);
        vDSP_vgen(&r2gainAtFrameStart, &r2gainAtFrameEnd, r2_gain, 1, framesInThisIteration);

        // adjust the volume in the buffers to fade between reverbs
        vDSP_vmul(r1_gain, 1, l_buffer_1, 1, l_buffer_1, 1, framesInThisIteration);
        vDSP_vmul(r1_gain, 1, r_buffer_1, 1, r_buffer_1, 1, framesInThisIteration);
        vDSP_vmul(r2_gain, 1, l_buffer_2, 1, l_buffer_2, 1, framesInThisIteration);
        vDSP_vmul(r2_gain, 1, r_buffer_2, 1, r_buffer_2, 1, framesInThisIteration);

        // sum the buffers into the outputs
    
        vDSP_vadd(l_buffer_1, 1, l_buffer_2, 1, &outputL[outputIndex], 1, framesInThisIteration);
        vDSP_vadd(r_buffer_1, 1, r_buffer_2, 1, &outputR[outputIndex], 1, framesInThisIteration);
        
        // update the number of frames left to process
        framesToProcess -= framesInThisIteration;
    }
}

void DoubleBufferedReverb::updateReverbSettings(){
    backReverb->setParameter(parameters);
}

void DoubleBufferedReverb::setListenerLocation(float* loc){
    Vector3D Ratio = Vector3D(loc[0],1.0f-loc[1]);
    printf("Ratio of listener is: %f %f \n", Ratio.x, Ratio.y);
    this->parameters.setListenerLocation(Ratio);
    updateReverbSettings();
    usleep(200000);
    flip();

}

void DoubleBufferedReverb::setSoundLocation(float* loc){
    Vector3D Ratio = Vector3D(loc[0],1.0f-loc[1]);
    this->parameters.setSoundLocation(Ratio);
    printf("Ratio of sound is: %f %f \n", Ratio.x, Ratio.y);
    updateReverbSettings();
    usleep(200000);
    flip();
}


void DoubleBufferedReverb::setSoundAndListenerLocation(float* locL, float* locS){
    Vector3D RatioL = Vector3D(locL[0],1.0f-locL[1]);
  //  printf("loc of listener is: %f %f \n", RatioL.x, RatioL.y);
    this->parameters.setListenerLocation(RatioL);
    Vector3D RatioS = Vector3D(locS[0],1.0f-locS[1]);
  //  printf("loc of sound is: %f %f \n", RatioS.x, RatioS.y);
    this->parameters.setSoundLocation(RatioS);
    updateReverbSettings();
    usleep(200000);
    flip();
}

void DoubleBufferedReverb::setRoomSize(float size){
    parameters.setRoomSize(size);
    updateReverbSettings();
    usleep(200000);
    flip();
}

void DoubleBufferedReverb::setWidth(float widthRatio){
    this->parameters.setWidth(widthRatio);
    updateReverbSettings();
    usleep(200000);
    flip();
}
void DoubleBufferedReverb::setLength(float ratio){
    this->parameters.setLength(ratio);
    updateReverbSettings();
    usleep(200000);
    flip();
}

void DoubleBufferedReverb::setRT60(float RT60){
    this->parameters.RT60 = RT60;
    updateReverbSettings();
    usleep(200000);
    flip();
}

void DoubleBufferedReverb::setRoomRayModel(bool roomRayModel){
    this->parameters.roomRayModelOn = roomRayModel;
    updateReverbSettings();
    usleep(200000);
    flip();
}

void DoubleBufferedReverb::flip(){
    // if reverb 1 is on the back buffer
    if (backReverb == &reverb1) {
        // move reverb 1 to the front buffer by increasing the gain
        r1_gainTarget = 1.0f;
        backReverb = &reverb2;
     
        
    } else {
        // move reverb 1 to the back buffer by decreasing the gain
        r1_gainTarget = 0.0f;
        backReverb = &reverb1;
    
    }

    
}

void DoubleBufferedReverb::setReverbONOFF(bool on){
    if (on){
        reverb2.reverbPortionOn = 1.0f;
        reverb1.reverbPortionOn = 1.0f;
    }
    else{
        reverb2.reverbPortionOn = 0.0f;
        reverb1.reverbPortionOn = 0.0f;
    }
}

void DoubleBufferedReverb::setDirectONOFF(bool on){
    if (on){
        reverb2.directPortionOn = 1.0f;
        reverb1.directPortionOn = 1.0f;
    }
    else{
        reverb2.directPortionOn = 0.0f;
        reverb1.directPortionOn = 0.0f;
    }
}
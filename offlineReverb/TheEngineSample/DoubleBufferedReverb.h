//
//  DoubleBufferedReverb.hpp
//  TheEngineSample
//
//  Created by Natalie Agus on 27/10/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//



#include <stdio.h>
#import "FDN.h"
#define DB_REVERB_BUFFER_SIZE 256
#import "Parameter.hpp"

class DoubleBufferedReverb
{
public:
    
    DoubleBufferedReverb(void);
    
    Parameter parameters = Parameter();
    
    FDN reverb1;
    FDN reverb2;
    FDN* backReverb;
    
    float roomSize = 0.15;
    float widthRatio = 0.5;
    float RT60 = 0.7f;
    Vector3D listenerLocation = Vector3D(0.5f, (1.0f/3.0f));
    Vector3D soundLocation = Vector3D(0.5f, (2.0f/3.0f));
    
    void setListenerLocation(float* loc);
    void setSoundLocation(float* loc);
    void setSoundAndListenerLocation(float* locL, float* locS);
    void setRoomSize(float size);

        void setWidth(float size);
        void setLength(float size);
    void setRT60(float RT60);
    void updateReverbSettings();
    void setRoomRayModel(bool roomRay);
    void processIFretlessBuffer(float* IOBuffer, size_t frames, float * outputL, float* outputR);
    
    
    void setReverbONOFF(bool on);
    void setDirectONOFF(bool onO);
private:
    void flip();

    float l_buffer_1[DB_REVERB_BUFFER_SIZE];
    float l_buffer_2[DB_REVERB_BUFFER_SIZE];
    float r_buffer_1[DB_REVERB_BUFFER_SIZE];
    float r_buffer_2[DB_REVERB_BUFFER_SIZE];
    float r1_gain[DB_REVERB_BUFFER_SIZE];
    float r2_gain[DB_REVERB_BUFFER_SIZE];
    float r1_gainTarget;

};


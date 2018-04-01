//
//  Parameter.hpp
//  TheEngineSample
//
//  Created by Natalie Agus on 28/10/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#ifndef Parameter_hpp
#define Parameter_hpp


#include <stdio.h>
#include "Vector3D.hpp"
#include <math.h>


#define RADIUSOFHEAD 0.084f //8cm radius of head
#define DMIN 0.1f

// If select this then change the smoothdelay into 1 at FDN.h
//#define ENERGY_BALANCE

typedef struct Parameter {
    

    //Initial values of parameter
    Parameter(){
//        this->roomSize = INITIALROOMSIZE * ROOMSIZE;
//        this->roomSizeRatio = INITIALROOMSIZE;
//
//        this->roomWidthRatio = INITIALWIDTH;
//        this->roomHeightRatio = INITIALLENGTH;
//        this->roomWidth = this->roomWidthRatio * this->roomSize;
//        this->roomHeight = this->roomHeightRatio * this->roomSize;
//        this->roomCeiling = ROOMCEILING;
        
//        Vector3D L = Vector3D(INITIALLISTENERX, INITIALLISTENERY);
//        setListenerLocation(L);
//        Vector3D S = Vector3D(INITIALSOUNDX, INITIALSOUNDY);
//        setSoundLocation(S);
//        this->listenerXYRatio = Vector3D(INITIALLISTENERX, INITIALLISTENERY);
//        this->soundXYRatio = Vector3D(INITIALSOUNDX, INITIALSOUNDY);
//    
//        this->RT60 = INITIALRT60;
        
//        this->directGain = INITIALDIRECTGAIN;
//        this->reverbGain = INITIALREVERBGAIN;
//        
        this->roomRayModelOn = true;
//        this->reflection = 0.95f;

        this->orientation = 0.0f;
        
        
//WATCH DMIN AND ENERGY DETECTED BY LISTENER
        
        this->roomType = 5;
        this->RT60 = 1.2f;
        this->roomWidth = 1.42f; //x
        this->roomHeight = 7.23f; //y
        this->roomCeiling = 2.61; //z
        
        
        this->soundSourceLoc = Vector3D(0.2f,5.23f,1.3f);
        this->listenerLoc = Vector3D(0.2f, 2.73f, 1.3f);
        
        
        this->hsffreq = 4000.f;
        this->hsfRT60 = 1.12f;
        this->energyReceived = 1.f;
        
        
        this->lsffreq = 125.f;
        this->lsfRT60 = 0.77f; //set to rt60 to deactivate
        
        //Usually not changed, this is to compensate ball to real HRTF
        this->lowpass = 9000; // set to 22000.f to deactivate
        this->bellPeak = 40000;
        this->bellBW = 1500.f;
        this->bellGain = 4.5f; // set to 1.0f to deactivate
        
        
        this->listenerLocLeftEar = Vector3D(cosf(orientation * M_PI / 180.f)*(- RADIUSOFHEAD) + sinf(orientation * M_PI / 180.f)*0.0f + listenerLoc.x, cosf(orientation * M_PI / 180.f)*0.0f-sinf(orientation * M_PI / 180.f)*(- RADIUSOFHEAD) + listenerLoc.y, listenerLoc.z);
        
        this->listenerLocRightEar =  Vector3D(cosf(orientation * M_PI / 180.f)*(+ RADIUSOFHEAD) + sinf(orientation * M_PI / 180.f)*0.0f + listenerLoc.x, cosf(orientation * M_PI / 180.f)*0.0f-sinf(orientation * M_PI / 180.f)*(+ RADIUSOFHEAD) + listenerLoc.y, listenerLoc.z);;

        printf("left ear: %f %f %f \n", listenerLocLeftEar.x,listenerLocLeftEar.y, listenerLocLeftEar.z);
         printf("right ear: %f %f %f \n", listenerLocRightEar.x,listenerLocRightEar.y, listenerLocRightEar.z);
      //  printf("RT60: %f, room width : %f, room length : %f\n", this->RT60, this->roomWidth, this->roomHeight);
       // printf("Listenerloc : %f %f ssloc : %f %f \n", this->listenerLoc.x, this->listenerLoc.y, this->soundSourceLoc.x, this->soundSourceLoc.y);
        
        
    }
    
    void setListenerLocation(Vector3D Ratio);
    void setSoundLocation(Vector3D Ratio);
    void setRoomSize(float size);
    void setWidth(float ratio);
    void setLength(float ratio);
    
//    Vector3D listenerXYRatio;
//    Vector3D soundXYRatio;
    
    Vector3D listenerLoc;
    Vector3D listenerLocLeftEar;
    Vector3D listenerLocRightEar;
    Vector3D soundSourceLoc;
    
    float orientation;
//    float roomSizeRatio;
    float RT60;
//    float roomSize;
    float roomWidth, roomHeight;
//    float roomWidthRatio, roomHeightRatio;
    float roomCeiling;
//    float directGain, reverbGain;
//    float reflection;
    float energyReceived;
    float hsfRT60;
    float hsffreq;
    
    float lsffreq;
    float lsfRT60;
    
    float lowpass;
    float bellPeak;
    float bellGain;
    float bellBW;
    
    int roomType;

    bool roomRayModelOn;
    
} Parameter;

#endif /* Parameter_hpp */

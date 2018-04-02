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



typedef struct Parameter {
    
    
    Parameter(){
        //default constructor
    }
    
    //Initial values of parameter
    Parameter(int roomType){
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
        if (roomType == 1){
            
            this->roomType = 1;
            this->RT60 = 2.11f;
            this->roomWidth = 1.95f; //x
            this->roomHeight = 5.52f; //y
            this->roomCeiling = 2.9f; //z
            
            this->soundSourceLoc = Vector3D(0.975f,3.6f,0.52f);
            this->listenerLoc = Vector3D(0.975f, 1.91f, 0.64f);
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.6f;
            this->energyReceived = 0.9f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 2){
            this->roomType = 2;
            this->RT60 = 2.09f;
            this->roomWidth = 1.95f; //x
            this->roomHeight = 5.52f; //y
            this->roomCeiling = 2.9f; //z
            
            this->soundSourceLoc = Vector3D(0.15f,2.38f,0.52f);
            this->listenerLoc = Vector3D(0.08f+0.084f, 0.9f, 0.64f);
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.4f;
            this->energyReceived = 0.9f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 3){
            this->roomType = 3;
            this->RT60 = 2.06f;
            this->roomWidth = 1.95f; //x
            this->roomHeight = 5.52f; //y
            this->roomCeiling = 2.9f; //z
            
            
            this->soundSourceLoc = Vector3D(1.8f,2.68f,0.52f);
            this->listenerLoc = Vector3D(1.75f, 1.15f, 0.64f);
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.4f;
            this->energyReceived = 0.9f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 4){
            this->roomType = 4;
            this->RT60 = 2.07f;
            this->roomWidth = 1.95f; //x
            this->roomHeight = 5.52f; //y
            this->roomCeiling = 2.9f; //z
            
            this->soundSourceLoc = Vector3D(1.8f,1.13f,0.52f);
            this->listenerLoc = Vector3D(1.786f, 0.08f, 0.64f);
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.4f;
            this->energyReceived = 0.9f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 5){
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
        }
        else if (roomType == 6){
            this->roomType = 6;
            this->RT60 = 1.13f;
            this->roomWidth = 1.42f; //x
            this->roomHeight = 7.23f; //y
            this->roomCeiling = 2.61; //z
            
            this->soundSourceLoc = Vector3D(1.22f,5.2f,0.18f);
            this->listenerLoc = Vector3D(1.22f, 3.2f, 0.18f);
            
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.1f;
            this->energyReceived = 1.f;
            
            
            this->lsffreq = 125.f;
            this->lsfRT60 = 0.83f; //set to rt60 to deactivate
            
            //Usually not changed, this is to compensate ball to real HRTF
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 7){
            this->roomType = 7;
            this->RT60 = 1.2f;
            this->roomWidth = 1.42f; //x
            this->roomHeight = 7.23f; //y
            this->roomCeiling = 2.61; //z
            this->soundSourceLoc = Vector3D(0.71f,5.23f,0.18f);
            this->listenerLoc = Vector3D(0.71f, 3.23f, 0.18f);
            
            
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
        }
        else if (roomType == 8){
            this->roomType = 8;
            this->RT60 = 2.35f;
            
            this->roomWidth = 2.68f; //x
            this->roomHeight = 2.75f; //y
            this->roomCeiling = 2.98f; //z
            
            this->soundSourceLoc = Vector3D(0.2f,2.1f,0.18f);
            this->listenerLoc = Vector3D(0.2f, 0.6f, 0.18f);
            
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.55;
            this->energyReceived = 1.f;
            
            
            this->lsffreq = 125.f;
            this->lsfRT60 = 1.44; //set to rt60 to deactivate
            
            //Usually not changed, this is to compensate ball to real HRTF
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 9){
            this->roomType = 9;
            this->RT60 = 2.13f;
            
            this->roomWidth = 2.68f; //x
            this->roomHeight = 2.75f; //y
            this->roomCeiling = 2.98f; //z
            
            this->soundSourceLoc = Vector3D(1.375f,2.18f,0.18f);
            this->listenerLoc = Vector3D(1.375f, 1.38f, 0.18f);
            
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.55;
            this->energyReceived = 1.f;
            
            
            this->lsffreq = 62.5f;
            this->lsfRT60 = 1.68f; //set to rt60 to deactivate
            
            //Usually not changed, this is to compensate ball to real HRTF
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 10){
            this->roomType = 10;
            this->RT60 = 2.12f;
            
            this->roomWidth = 2.75f; //x
            this->roomHeight = 2.68f; //y
            this->roomCeiling = 2.98f; //z
            
            
            this->soundSourceLoc = Vector3D(1.375f,2.18f,0.18f);
            this->listenerLoc = Vector3D(1.375f, 0.98f, 0.18f);
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.55;
            this->energyReceived = 1.f;
            
            
            this->lsffreq = 125.f;
            this->lsfRT60 = 1.44; //set to rt60 to deactivate
            
            //Usually not changed, this is to compensate ball to real HRTF
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 11){
            this->roomType = 11;
            this->RT60 = 2.2f;
            
            this->roomWidth = 2.75f; //x
            this->roomHeight = 2.68f; //y
            this->roomCeiling = 2.98f; //z
            
            
            this->soundSourceLoc = Vector3D(2.55f,2.18f,0.18f);
            this->listenerLoc = Vector3D(2.55f, 0.85f, 0.18f);
            
            this->hsffreq = 4000.f;
            this->hsfRT60 = 1.55;
            this->energyReceived = 1.f;
            
            
            this->lsffreq = 125.f;
            this->lsfRT60 = 1.44; //set to rt60 to deactivate
            
            //Usually not changed, this is to compensate ball to real HRTF
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 4.5f; // set to 1.0f to deactivate
        }
        else if (roomType == 12){
            
            //offline Version
            this->roomType = 12;
            
            this->RT60 = 0.98f;
            this->roomWidth = 10.8f; //x
            this->roomHeight = 10.9f; //y
            this->roomCeiling = 3.5f; //z
            this->listenerLoc = Vector3D(5.45f,3.f, 1.2f);
            this->soundSourceLoc = Vector3D(5.3f, 10.f,1.2f);
            
            this->hsffreq = 8000.f;
            this->hsfRT60 = 0.6f;
            this->energyReceived = 0.5f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 22000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 1.f; // set to 1.0f to deactivate
            
        }
        else if (roomType == 13){
            
            //offline Version
            this->roomType = 13;
            this->RT60 = 0.33f;
            this->roomWidth = 8.0f; //x
            this->roomHeight = 5.0f; //y
            this->roomCeiling = 3.1f; //z
            this->soundSourceLoc = Vector3D(7.0f, 4.f, 1.2f);
            this->listenerLoc = Vector3D(7.0f, 2.55,1.2f);
            
            this->hsffreq = 8000.f;
            this->hsfRT60 = 0.25f;
            this->energyReceived = 0.2f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 22000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 1.f; // set to 1.0f to deactivate
        }
        else if (roomType == 14){
            //offline Version
            this->roomType = 14;
            this->RT60 = 0.585f;
            this->roomWidth = 5.00f; //x
            this->roomHeight = 6.4f; //y
            this->roomCeiling = 2.9f; //z
            this->listenerLoc = Vector3D(2.5f, 5.5f,1.2f);
            this->soundSourceLoc = Vector3D(2.5f, 3.5f,1.2f);
            
            this->hsffreq = 8000.f;
            this->hsfRT60 = 0.28f;
            this->energyReceived = 0.18f;
            
            
            this->lsffreq = 125.f;
            this->lsfRT60 = 0.8f; //set to rt60 to deactivate
            
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 1.f; // set to 1.0f to deactivate
            
        }
        else if (roomType == 15){
            //offline Version
            this->roomType = 15;
            
            this->RT60 = 0.6f;
            this->roomWidth = 5.00f; //x
            this->roomHeight = 6.4f; //y
            this->roomCeiling = 2.9f; //z
            this->listenerLoc = Vector3D(2.5f, 5.5f,1.2f);
            this->soundSourceLoc = Vector3D(2.5f, 4.5f,1.2f);
            
            this->hsffreq = 8000.f;
            this->hsfRT60 = 0.28f;
            this->energyReceived = 0.25f;
            
            
            this->lsffreq = 125.f;
            this->lsfRT60 = 0.8f; //set to rt60 to deactivate
            
            this->lowpass = 9000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 1.f; // set to 1.0f to deactivate
        }
        else if (roomType == 16){
            
            //offline Version
            this->roomType = 16;
            
            this->RT60 = 0.92f;
            this->roomWidth = 10.8f; //x
            this->roomHeight = 10.9f; //y
            this->roomCeiling = 3.5f; //z
            this->listenerLoc = Vector3D(5.45f,3.f, 1.2f);
            this->soundSourceLoc = Vector3D(5.3f, 7.f,1.2f);
            
            this->hsffreq = 8000.f;
            this->hsfRT60 = 0.6f;
            this->energyReceived = 0.5f;
            
            
            this->lsffreq = 200.f;
            this->lsfRT60 = RT60; //set to rt60 to deactivate
            
            this->lowpass = 22000; // set to 22000.f to deactivate
            this->bellPeak = 40000;
            this->bellBW = 1500.f;
            this->bellGain = 1.f; // set to 1.0f to deactivate
            
        }
        else{
            printf("UNDEFINED ROOM TYPE : %i \n" , roomType);
            throw -1;
        }
        
        
        this->listenerLocLeftEar = Vector3D(cosf(orientation * M_PI / 180.f)*(- RADIUSOFHEAD) + sinf(orientation * M_PI / 180.f)*0.0f + listenerLoc.x, cosf(orientation * M_PI / 180.f)*0.0f-sinf(orientation * M_PI / 180.f)*(- RADIUSOFHEAD) + listenerLoc.y, listenerLoc.z);
        
        this->listenerLocRightEar =  Vector3D(cosf(orientation * M_PI / 180.f)*(+ RADIUSOFHEAD) + sinf(orientation * M_PI / 180.f)*0.0f + listenerLoc.x, cosf(orientation * M_PI / 180.f)*0.0f-sinf(orientation * M_PI / 180.f)*(+ RADIUSOFHEAD) + listenerLoc.y, listenerLoc.z);;

        printf("left ear: %f %f %f \n", listenerLocLeftEar.x,listenerLocLeftEar.y, listenerLocLeftEar.z);
         printf("right ear: %f %f %f \n", listenerLocRightEar.x,listenerLocRightEar.y, listenerLocRightEar.z);
      //  printf("RT60: %f, room width : %f, room length : %f\n", this->RT60, this->roomWidth, this->roomHeight);
       // printf("Listenerloc : %f %f ssloc : %f %f \n", this->listenerLoc.x, this->listenerLoc.y, this->soundSourceLoc.x, this->soundSourceLoc.y);
        
        setSmallestAndLargestDim();
        
    }
    
    void setListenerLocation(Vector3D Ratio);
    void setSoundLocation(Vector3D Ratio);
    void setRoomSize(float size);
    void setWidth(float ratio);
    void setLength(float ratio);
    void setSmallestAndLargestDim();
    
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
    
    float smallestDim;
    float largestDim;
    
    int roomType;

    bool roomRayModelOn;
    
} Parameter;

#endif /* Parameter_hpp */

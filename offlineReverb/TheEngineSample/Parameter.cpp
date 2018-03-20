//
//  Parameter.cpp
//  TheEngineSample
//
//  Created by Natalie Agus on 28/10/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#include "Parameter.hpp"

void Parameter::setListenerLocation(Vector3D Ratio){
    printf("Setting new listener location \n");
//    this->listenerXYRatio = Ratio;
//   // printf("Ratio Listener : x %f, y %f \n ", Ratio.x, Ratio.y);
//    this->listenerLoc = Vector3D(this->roomWidth * Ratio.x, this->roomHeight * Ratio.y);
//    this->listenerLocLeftEar.x = this->listenerLoc.x - RADIUSOFHEAD;
//    this->listenerLocLeftEar.y = this->listenerLoc.y;
//    this->listenerLocRightEar.x = this->listenerLoc.x + RADIUSOFHEAD;
//    this->listenerLocRightEar.y = this->listenerLoc.y;
  //  printf("List loc is now : %f %f \n", listenerLoc.x, listenerLoc.y);
}

void Parameter::setSoundLocation(Vector3D Ratio){
        printf("Setting new source location \n");
//    this->soundXYRatio = Ratio;
//    //    printf("Ratio Soundsource : x %f, y %f \n ", Ratio.x, Ratio.y);
//    this->soundSourceLoc = Vector3D(this->roomWidth * Ratio.x, this->roomHeight * Ratio.y);
//      //  printf("sound loc is now : %f %f \n", soundSourceLoc.x, soundSourceLoc.y);
}

void Parameter::setRoomSize(float size){
        printf("Setting new room size \n");
//    this->roomSizeRatio = size;
//    this->roomSize = this->roomSizeRatio * ROOMSIZE;
//    this->roomWidth = this->roomSize * roomWidthRatio;
//    this->roomHeight = this->roomSize * roomHeightRatio;
//    setListenerLocation(this->listenerXYRatio);
//    setSoundLocation(this->soundXYRatio);
  //  printf("Room size is now: %f\n", roomSize);
}
void Parameter::setLength(float ratio){
        printf("Setting new room length \n");
    
//    
//    this->roomHeightRatio = ratio;
//    this->roomHeight = this->roomHeightRatio * this->roomSize;
//    
//    setListenerLocation(this->listenerXYRatio);
//    setSoundLocation(this->soundXYRatio);
   // printf("New roomWidth: %f, new roomHeight: %f, room size: %f\n", roomWidth, roomHeight, roomSize);
}

void Parameter::setWidth(float ratio){
        printf("Setting new room width \n");
//
//
//    this->roomWidthRatio = ratio;
//    this->roomWidth = this->roomWidthRatio * this->roomSize;
//    this->roomHeight = this->roomHeightRatio * this->roomSize;
//    
//    setListenerLocation(this->listenerXYRatio);
//    setSoundLocation(this->soundXYRatio);
 //   printf("New roomWidth: %f, new roomHeight: %f, room size: %f\n", roomWidth, roomHeight, roomSize);
}
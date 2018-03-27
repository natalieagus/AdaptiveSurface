// offlineReverb.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "FDN.h"
#include <ctime>
#include <string>
#include <fstream>
#include <cstdint>
#include "assert.h"
#include <iostream>
#include <fstream>
#include "Vector3D.hpp"
#include "Ray.hpp"
#include "CuboidGroup.hpp"
#include <set>
#include <map>


#define FS 44100

using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::string;
using namespace std;

void saveImpulse(int type, int samples, std::ofstream* ofLeft, std::ofstream* ofRight){
    
//    clock_t begin = clock();
    std::string filenameL = "impulse";
    if (type > 0) filenameL += "H";
    else filenameL += "C";
    filenameL += std::to_string(abs(type));
    filenameL += "Left.csv";
    
    std::string filenameR = "impulse";
    if (type > 0) filenameR += "H";
    else filenameR += "C";
    filenameR += std::to_string(abs(type));
    filenameR += "Right.csv";
    
    
    ofLeft->open(filenameL);
    ofRight->open(filenameR);
    
    FDN reverb = FDN();
    reverb.impulseResponse(samples, ofLeft, ofRight);
    
    std::cout << "impulse saved for type " << type << ".\n";
    ofLeft->close();
    ofRight->close();
    
//    clock_t end = clock();
//    double elapsed_msecs = double(end - begin) / CLOCKS_PER_SEC * 1000.f;
//    
//    printf("Time elapsed: %f ms\n", elapsed_msecs);
}




int main(int argc, char* argv[])
{
    
    //Saving IR for JAES2
//    float impulseLength = 5.0f;
//
//    std::ofstream impulseLeft;
//    std::ofstream impulseRight;
//    std::ifstream inFile;
//
//    saveImpulse(16, FS*impulseLength, &impulseLeft, &impulseRight);
//
//    std::cout << "\ndone.\n";
//
//    Cuboid cube = Cuboid();
//    cube.segmentCube(9);
    
    
    //init CuboidGroup
    CuboidGroup CG = CuboidGroup(1, 1, 1, 25);
    
    // testing ray-plane intersection
    Vector3D listener = Vector3D(0.5, 0.5, 0.5);
    Vector3D S = Vector3D(0.8, 0.5, 0.5);
    
    int numRays = 12;
    int *delayValues = new int[numRays];
    CG.assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(listener, numRays);
    
    CG.getDelayValues(delayValues, listener, listener, S, 44100);
    
    for (int i = 0; i<numRays; i++){
        printf("Delay %i : %i\n", i, delayValues[i]);
    }
    
    
    
    return 0;
    
    
}





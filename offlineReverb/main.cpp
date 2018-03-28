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
    
    clock_t begin = clock();
    
//    //Saving IR for JAES2
//    float impulseLength = 5.0f;
//
//    std::ofstream impulseLeft;
//    std::ofstream impulseRight;
//    std::ifstream inFile;
//
//    saveImpulse(16, FS*impulseLength, &impulseLeft, &impulseRight);
//
//    std::cout << "\ndone.\n";

    Cuboid cube = Cuboid();
    cube.segmentCube(9);
    
    
//    //init CuboidGroup
    CuboidGroup CG = CuboidGroup(5, 5, 10, 12*12);
//
//    // testing ray-plane intersection
    Vector3D listener = Vector3D(0.2, 0.2, 5);
    Vector3D source = Vector3D(2.5, 3, 5);
//
    int numRays = 32;
    int *delayValues = new int[numRays];
    int rays_without_patches = CG.assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(listener, numRays);

    std::cout << " \n Rays without patches: " << rays_without_patches << std::endl;

    float *feedbackTapGains = new float[numRays];
    for (int i = 0; i<numRays; i++){
        feedbackTapGains[i] = 1.0f;
    }

    Gains G = Gains(0.1f, feedbackTapGains, 0.5f, 1.0f, &CG);

    float difference_energy = G.calculateGainsGroup(&CG, listener, source);

    printf("Difference energy: %f \n", difference_energy);

    clock_t end = clock();
    double elapsed_msecs = double(end - begin) / CLOCKS_PER_SEC * 1000.f;

    printf("Time elapsed: %f ms\n", elapsed_msecs);


    
    CG.getDelayValues(delayValues, listener, listener, source, 44100);

    for (int i = 0; i<numRays; i++){
        printf("Delay %i : %i\n", i, delayValues[i]);
    }

    
    
    return 0;
    
    
}





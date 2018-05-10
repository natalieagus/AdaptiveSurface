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

void saveImpulse(int type, int samples, std::ofstream* ofLeft, std::ofstream* ofRight, int roomType){
    
    FDN reverb = FDN(roomType);
    
//    clock_t begin = clock();
    std::string filename = std::to_string(abs(roomType));
    filename += "roomImpulse";
    std::string filenameL = filename + "Left.csv";
    std::string filenameR = filename + "Right.csv";
    
    
    ofLeft->open(filenameL);
    ofRight->open(filenameR);
    
  
    
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
    
//    clock_t begin = clock();
    
//    //Saving IR for JAES2
    for (int i = 1; i<=1; i ++){
    float impulseLength = 4.0f;

    std::ofstream impulseLeft;
    std::ofstream impulseRight;
    std::ifstream inFile;

    saveImpulse(16, FS*impulseLength, &impulseLeft, &impulseRight, i);

    std::cout << "\ndone for room type " << i <<".\n";
    }
////
//    Cuboid cube = Cuboid();
//    cube.segmentCube(9);
//
//
////    //init CuboidGroup
//    CuboidGroup CG = CuboidGroup(1.95, 5.52, 2.9, 12*12);
////
////    // testing ray-plane intersection
//    Vector3D listener = Vector3D(0.08f+0.084f, 0.9f, 0.64f);
//    Vector3D source = Vector3D(0.15f,2.38f,0.52f);
//
//
//
//
//    int numRays = 64;
//    int *delayValues = new int[numRays];
//    //set them all to zero
//    memset(delayValues, 0, numRays*sizeof(float));
//    int rays_without_patches = CG.assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(listener, numRays);
//
//    std::cout << " \n Rays without patches: " << rays_without_patches << std::endl;
//
//    float *feedbackTapGains = new float[numRays];
//    for (int i = 0; i<numRays; i++){
//        feedbackTapGains[i] = 1.0f;
//    }
//
//    Gains G = Gains(0.1f, feedbackTapGains, 0.5f, 1.0f, &CG);
//
//    float difference_energy = G.calculateGainsGroup(&CG, listener, source);
//
//    printf("Difference energy: %f \n", difference_energy);
//
//    clock_t end = clock();
//    double elapsed_msecs = double(end - begin) / CLOCKS_PER_SEC * 1000.f;
//
//    printf("Time elapsed: %f ms\n", elapsed_msecs);
//
//
//
//    CG.getDelayValues(delayValues, listener, listener, source, 44100);
//
//    for (int i = 0; i<numRays; i++){
//        printf("Delay %i : %i\n", i, delayValues[i]);
//    }
//
//
    
    return 0;
    
    
}





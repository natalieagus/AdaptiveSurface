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

/*Checks if point M is within bounded plane P
 *@returns  true/false
 */
bool isWithinRectangularPlane(Plane3D P, Vector3D M){

    if (
         (0 <= M.subtract(P.corner).dotProduct(P.S1) && M.subtract(P.corner).dotProduct(P.S1) <= P.S1.dotProduct(P.S1)) &&
         (0 <= M.subtract(P.corner).dotProduct(P.S2) && M.subtract(P.corner).dotProduct(P.S2) <= P.S2.dotProduct(P.S2))
        ){
        std::cout << " within ";
        return true;
    }
    
    std::cout << " not within ";
    return false;
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
    
    
    
    //testing lateral method
    Vector3D listener = Vector3D(0,0.1,0.5);
    
    Vector3D c = Vector3D(0,0,0);
    Vector3D s1 = Vector3D(0,0,1);
    Vector3D s2 = Vector3D(1,0,0);
    
    Plane3D P = Plane3D(c, s1, s2);
    
    bool within_plane = isWithinRectangularPlane(P, listener);
    
    std::cout << " Is listener within plane? Ans: " << within_plane  << "\n";
    
////    int tilesPerSide = 4;
//    Cuboid cube = Cuboid(1, 1, 1);
////    cube.sliceCubeLateral(tilesPerSide, 50, 16, listener);
//
//    //testing spiral method
//    int n = 30;
//    Vector3D* out = new Vector3D[n];
//    cube.bauersMethodOnListener(n, out, listener);
    
  
    return 0;
    
    
}





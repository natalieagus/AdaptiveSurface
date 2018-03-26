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
    CuboidGroup CG = CuboidGroup(1, 1, 1, 4);
    
    // testing ray-plane intersection
    Vector3D listener = Vector3D(2, 0.5, 0.5);
    
    Ray r = Ray(listener, Vector3D(0, -1, 0));
    
    Vector3D c = Vector3D(0,0,0);
    Vector3D s1 = Vector3D(0,0,1);
    Vector3D s2 = Vector3D(1,0,0);
    
    Plane3D P = Plane3D(c, s1, s2);
    
    //    bool within_plane = isWithinRectangularPlane(P, listener);
    
    float u = 0.f;
    bool intersects_plane = CG.rayPlaneIntersection(P, r, &u);
    std::cout << " Is ray intersecting plane? Ans: " << intersects_plane  << "\n";
    
    
    
    
    //    Vector3D p_normal = P.getNormal();
    //
    //    printf("Plane's normal is : %f %f %f \n", p_normal.x, p_normal.y, p_normal.z);
    
    if (intersects_plane) {
        
        Vector3D intersection = r.get_vector(u);
        printf("Intersects at coordinate: {%f, %f, %f} \n", intersection.x, intersection.y, intersection.z);
        bool within_plane = CG.isWithinRectangularPlane(P, r.get_vector(u));
        
        std::cout << " Is intersection within plane? Ans: " << within_plane  << "\n";
    }

    
    return 0;
    
    
}





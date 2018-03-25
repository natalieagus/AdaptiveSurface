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



/*Checks if point M is within bounded rectangle P
 *Condition:    M has to be on the same plane as P
 *              Only Use this after rayPlaneIntersection returns true
 *@returns  true/false
 */
bool isWithinRectangularPlane(Plane3D P, Vector3D M){

    //check if M is on plane P
    Vector3D MP = P.corner.subtract(M);
    float d_prod = MP.dotProduct(P.getNormal());
    
//    printf("d prod %f \n", fabs(d_prod - 0));
    
    if (fabs(d_prod - 0) >= 0.00001){
        printf("M is not on the plane P \n");
        return false;
    }
    
    
    Vector3D P1 = P.corner.add(P.S1);
    Vector3D P2 = P1.add(P.S2);
    Vector3D P3 = P.corner.add(P.S2);
    Vector3D P4 = P.corner;
    
    Vector3D V3 = P4.subtract(P3);
    Vector3D V1 = P2.subtract(P1);
    Vector3D V4 = M.subtract(P1);
    Vector3D V5 = M.subtract(P3);
    
    float V1_dot_V4 = V1.dotProduct(V4);
    float V3_dot_V5 = V3.dotProduct(V5);
    
    if ( V1_dot_V4 >= 0 && V3_dot_V5 >= 0){
        std::cout << " \n within \n ";
        return true;
    }
    
    std::cout << " \n is not within \n ";
    return false;
}

bool rayPlaneIntersection(Plane3D p, Ray r, float* u){
    
    //Check if ray origins is at plane's corner
    if (fabs(r.p.subtract(p.corner).magnitude()) < 0.00001){
        *u = 0;
        std::cout << "\nt : " << *u << " \n";
        std::cout << " intersects at origin = plane corner";
        return true;
    }
    
    float denominator = p.getNormal().dotProduct(r.d);
    
    std::cout << "\ndenominator : " << denominator << " \n";
    if (fabs(denominator) > 0.000001){
        *u = p.corner.subtract(r.p).dotProduct(p.getNormal()) / denominator;
        std::cout << "\nu : " << *u << " \n";
        if (*u >= 0){
            std::cout << " intersects ";
            return true;
        }
    }
    
    std::cout << " does not intersect ";
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
    
    
    
    //random workspace for tests
    //getting unique elements from set
    int *a = new int[10];
    for (int i = 0; i<10;i++){
        a[i] = i;
    }
    
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    
    
    std::set<std::pair<int, int>> items;
    
    for (int i = 0; i<5;i++){
        items.insert(make_pair(2, i));
    }
    
    for (int i = 5; i<10;i++){
        items.insert(make_pair(3, i));
    }
    
    
    for (std::set<std::pair<int, int>>::iterator it=items.begin(); it!=items.end(); ++it)
    {
        std::cout << it->first << " " << it->second <<std::endl;
    }
    

    
    return 0;
    
    
}





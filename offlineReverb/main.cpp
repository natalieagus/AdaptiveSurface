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


bool isWithinRectangularPlane2D(float ABX, float ABY, float ADX, float ADY, float AMX, float AMY){
    
    std::cout << ABX << " " << ABY << " " << ADX << " " << ADY << " " << AMX << " " << AMY << " \n ";
    
    float AB_dot_AB = ABX * ABX + ABY * ABY;
    float AD_dot_AD = ADX * ADX + ADY * ADY;
    float AM_dot_AB = AMX * ABX + AMY * ABY;
    float AM_dot_AD = AMX * ADX + AMY * ADY;
    
    if ( AB_dot_AB > 0 && AD_dot_AD > 0){
    
        std::cout << AM_dot_AB << " " << AB_dot_AB << " " << AM_dot_AD << " " << AD_dot_AD;
    
        if ( 0 <= AM_dot_AB && AM_dot_AB <= AB_dot_AB && 0 <= AM_dot_AD && AM_dot_AD <= AD_dot_AD ){
            printf("YES");
            return true;
        }
        printf("NO");
        return false;
    }
    
    return false;
//    else if ( AB_dot_AB > 0 ){
//        //AB is a line
//        dxc = AMX - ABX;
//        dyc = AMY - ABY;
//
//        dxl = point2.x - point1.x;
//        dyl = point2.y - point1.y;
//
//        cross = dxc * dyl - dyc * dxl;
//    }
    
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
        printf("M is not on the plane P");
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
    
    
    
    //testing lateral method
    Vector3D listener = Vector3D(0.5, 0.5, 0.5);
    
    Ray r = Ray(listener, Vector3D(0, -1, 0));
    
    Vector3D c = Vector3D(0,0,0);
    Vector3D s1 = Vector3D(0,0,1);
    Vector3D s2 = Vector3D(1,0,0);
    
    Plane3D P = Plane3D(c, s1, s2);
    
//    bool within_plane = isWithinRectangularPlane(P, listener);
    
    float u = 0.f;
    bool intersects_plane = rayPlaneIntersection(P, r, &u);
    std::cout << " Is ray intersecting plane? Ans: " << intersects_plane  << "\n";




//    Vector3D p_normal = P.getNormal();
//
//    printf("Plane's normal is : %f %f %f \n", p_normal.x, p_normal.y, p_normal.z);

    if (intersects_plane) {

        Vector3D intersection = r.get_vector(u);
        printf("Intersects at coordinate: {%f, %f, %f} \n", intersection.x, intersection.y, intersection.z);
        bool within_plane = isWithinRectangularPlane(P, r.get_vector(u));

        std::cout << " Is intersection within plane? Ans: " << within_plane  << "\n";
    }
    
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





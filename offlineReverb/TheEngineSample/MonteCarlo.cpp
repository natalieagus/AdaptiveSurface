//
//  MonteCarlo.c
//  surfaceIntegration
//
//  Created by Hans on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

//
//#ifdef __cplusplus
//extern "C" {
//#endif

#include "MonteCarlo.hpp"
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <random>
using namespace std;




Vector3D crossProduct(Vector3D a, Vector3D b){
    Vector3D out = Vector3D(a.y*b.z - a.z*b.y,
                            a.z*b.x - a.x*b.z,
                            a.x*b.y - a.y*b.x);
    return out;
    
}




float dotProduct(Vector3D a, Vector3D b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}




float norm(Vector3D x){
    return sqrtf(x.x*x.x + x.y*x.y + x.z*x.z);
}



Vector3D normalize(Vector3D x){
    float n = norm(x);
    Vector3D out = Vector3D(x.x/n, x.y/n, x.z/n);
    return out;
}



Vector3D scalarProduct(float a, Vector3D x){
    Vector3D out = Vector3D(a*x.x, a*x.y, a*x.z);
    return out;
}



// return the projection of b in the direction of a
Vector3D projection(Vector3D b, Vector3D a){
    // get a unit vector in the direction of a
    Vector3D an = normalize(a);
    
    // dot product an with b
    float dp = dotProduct(an, b);
    
    // projection of b in the direction of an
    return scalarProduct(dp, an);
}



// a - b
Vector3D subtractV3(Vector3D a, Vector3D b){
    Vector3D out = Vector3D(a.x - b.x, a.y - b.y, a.z - b.z);
    return out;
}




// returns a vector orthogonal to a in the a,b plane
Vector3D orthogonalTo(Vector3D a, Vector3D b){
    // find the projection of b in the direction of a
    Vector3D bProjA = projection(b,a);
    
    // remove the projection of b from b
    return subtractV3(b,bProjA);
}



// returns true if a and b are colinear
bool colinearQ(Vector3D a, Vector3D b){
    Vector3D cp = crossProduct(a,b);
    return (cp.x * cp.y * cp.z != 0.0f);
}



// random number between 0 and max
float randomFloat(float min,float max){
//            srand(time(NULL));
    assert(min < max);
    return min + (float)rand()/(float)(RAND_MAX/(max-min));
}



Vector3D add3(Vector3D a, Vector3D b, Vector3D c){
    Vector3D out = Vector3D(a.x+b.x+c.x, a.y+b.y+c.y, a.z+b.z+c.z);
    return out;
}



// generate a random point in the a,b plane within radius units of centre
Vector3D randomPoint(Vector3D centre, Vector3D a, Vector3D b, float radius){
    // generate two vectors of length <= radius
    // these vectors are in the a,b plane
    Vector3D v1 = scalarProduct(randomFloat(-radius,radius),normalize(a));
    Vector3D v2 = scalarProduct(randomFloat(-radius,radius),normalize(b));
    
    // return a random point near c
    return add3(centre,v1, v2);
}



// returns true if p1 and p2 are on the same side of the line ab
// (I don't understand what they mean by "same side" in 3d space)
// http://blackpawn.com/texts/pointinpoly/
bool sameSideQ(Vector3D p1, Vector3D p2, Vector3D a, Vector3D b){
    Vector3D cp1 = crossProduct(subtractV3(b,a), subtractV3(p1,a));
    Vector3D cp2 = crossProduct(subtractV3(b,a), subtractV3(p2,a));
    if (dotProduct(cp1, cp2) >= 0.0) return true;
    return false;
}



// returns true if p is on the triangle a, b, c
bool pointOnTriangleQ(Vector3D p, Vector3D a,Vector3D b, Vector3D c){
    if (sameSideQ(p,a,b,c) &&
        sameSideQ(p,b, a,c) &&
        sameSideQ(p,c, a,b)){
        return true;
    }
    return false;
}




Vector3D addV3(Vector3D a, Vector3D b){
    Vector3D out = Vector3D(a.x+b.x, a.y+b.y, a.z+b.z);
    return out;
}





void randomPointsOnTriangle(Vector3D c, Vector3D s1, Vector3D s2, Vector3D* points, size_t numPoints){

    // s1 and s2 should not be co-linear
    //    printf("S1 %f %f %f S2  %f %f %f \n", s1.x, s1.y, s1.z, s2.x, s2.y, s2.z);
    assert(!colinearQ(s1,s2));
    
    // get a unit vector that is orthogonal to s1, but in the same plane as s1 and s2
    Vector3D s2o = orthogonalTo(s1, s2);
    
    // get the max length of the s1 and s2. This is the radius of the circle in which we will generate random points
    // get the length of each vector
    float n1 = norm(s1);
    float n2 = norm(s2);
    // take the max of n1, n2
    float radius = n1 > n2 ? n1 : n2;
    
    // now we can generate random points in the a,b plane staying within a distance of radius from c
    size_t i=0;
    while(i<numPoints){
        // generate a random point in the a,b plane near c
        Vector3D rnd = randomPoint(c,s1,s2o,radius);
        
        // check if that point is in the triangle
        bool onTriangle = pointOnTriangleQ(rnd,c,addV3(c,s1),addV3(c,s2));
        
        // if it is, save it and increment the count
        if(onTriangle){
            points[i] = rnd;
            i++;
        }
    }
}



void randomPointsOnRectangle(Vector3D c, Vector3D s1, Vector3D s2, Vector3D* points, size_t numPoints){
    
    //Set delay length for delays without output and input tap
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(10);    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_real_distribution<float> uni_s1(0, s1.magnitude()); // guaranteed unbiased
    std::uniform_real_distribution<float> uni_s2(0, s2.magnitude()); // guaranteed unbiased
    
    for (int i = 0; i<numPoints; i++){
        
        float scalar_s1 = uni_s1(rng);
        float scalar_s2 = uni_s2(rng);
        
//        std::cout << scalar_s1 << " " << scalar_s2 << std::endl;
        points[i] = c.add(s1.scalarMult(scalar_s1).add(s2.scalarMult(scalar_s2)));
        
//        printf("{%f, %f, %f},", points[i].x, points[i].y, points[i].z);
        
    }
    
//    // we are going to do this with two triangles.  How many points for each?
//    size_t t1Points = floor(numPoints/2);
//    size_t t2Points = numPoints - t1Points;
//
//    //  printf("%f %f %f , %f %f %f,  %f %f %f \n", c.x, c.y, c.z, s1.x, s1.y, s1.z, s2.x, s2.y, s2.z);
//    // do the first triangle
//    randomPointsOnTriangle(c, s1, s2, points, t1Points);
//
//    // printf("triangle1  done");
//    // where is the opposite corner of the rectangle?
//    Vector3D c2 = Vector3D(c.x+s1.x+s2.x, c.y+s1.y+s2.y, c.z+s1.z+s2.z);
//
//    // get vectors pointing in the opposite directions
//    Vector3D s3 = Vector3D(-s1.x,-s1.y,-s1.z);
//    Vector3D s4 = Vector3D(-s2.x,-s2.y,-s2.z);
//
//    // do the second triangle
//    randomPointsOnTriangle(c2,s3,s4, points + t1Points, t2Points);
//
//    // printf("triangle2  done");
    
    
}



//#ifdef __cplusplus
//}
//#endif

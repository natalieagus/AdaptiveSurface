//
//  MonteCarlo.h
//  surfaceIntegration
//
//  Created by Hans on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

//#ifdef __cplusplus
//extern "C" {
//#endif

#ifndef MonteCarlo_hpp
#define MonteCarlo_hpp

#include <stdio.h>
#include "Vector3D.hpp"
#include "Plane3D.hpp"
#include "Cuboid.hpp"




void randomPointsOnTriangle(Vector3D c, Vector3D s1, Vector3D s2, Vector3D* points, size_t numPoints);


void randomPointsOnRectangle(Vector3D c, Vector3D s1, Vector3D s2, Vector3D* points, size_t numPoints);
    



#endif /* MonteCarlo_hpp */
    
//#ifdef __cplusplus
//}
//#endif

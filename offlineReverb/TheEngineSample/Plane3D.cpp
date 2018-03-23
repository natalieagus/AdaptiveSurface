//
//  Plane3D.cpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "Plane3D.hpp"

Vector3D Plane3D:: getMidpoint(){
    Vector3D out = corner.add(S1.scalarMult(0.5f));
    out = out.add(S2.scalarMult(0.5f));
    return out;
}

float Plane3D:: getArea(){
    return S1.magnitude() * S2.magnitude();
}

Vector3D Plane3D::getNormal(){
    return S1.crossProduct(S2);
}

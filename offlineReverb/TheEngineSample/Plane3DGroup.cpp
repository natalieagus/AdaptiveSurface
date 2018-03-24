//
//  Plane3DGroup.cpp
//  offlineReverb
//
//  Created by Natalie Agus on 23/3/18.
//  Copyright © 2018 Hans. All rights reserved.
//

#include "Plane3DGroup.hpp"


/*Only use this when a single patch is divided into several rays
 *Valid only when there's only 1 element in this class
 *@param    newDivisor: number of rays sharing this patch
 */
void Plane3DGroup::updateDivisor(int newDivisor){
    this->area = (this->area * (float) this->divisor) / (float) newDivisor;
    this->divisor = newDivisor;
}

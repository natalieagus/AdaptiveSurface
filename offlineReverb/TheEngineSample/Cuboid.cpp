//
//  Cuboid.cpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "Cuboid.hpp"


using namespace std;

/*Emphasize patch decomposition on the lateral plane
 *
 *@param tilesPerSide          = the number of even segmentation per side needed
 *@param slices                = total number of patches required, typically 32, or 64, or 128 if Hadamard FDN is used
 *@param channels              = azimuthal segmentation
 *@param listener              = listener's location
 */
void Cuboid::sliceCubeLateral(int tilesPerSide, int slices, int channels, Vector3D listener){
    //Assert necessary condition about the tilesPerSide, slices, and channels
    assert(slices >= ( channels + 4) + (tilesPerSide * 6));
    
    //set the tiles per side for the room, this number has to be able to be square-rooted
//    int tilesPerSide = 9;
    segmentCube(tilesPerSide);
    
    //this->segmentedSides will now have tilesPerSide*tilesPerSide*6 surfaces
    int number_of_tiles_after_even_segmentation = 6*tilesPerSide;
    int number_of_ceiling_floor_surfaces = 2*tilesPerSide;
    int number_of_walls_out_array_size = slices - number_of_ceiling_floor_surfaces;
    int number_of_non_ceiling_floor_surfaces = number_of_tiles_after_even_segmentation - number_of_ceiling_floor_surfaces;

    
    //create temp array for walls_in, which is all surfaces without ceiling or floor
    Plane3D *walls_in = new Plane3D[number_of_non_ceiling_floor_surfaces];
    Plane3D *walls_ceiling_and_floor = new Plane3D[number_of_ceiling_floor_surfaces];
    
    //copy ceiling and floor to walls_ceiling_and_floor array
    std::memcpy(walls_ceiling_and_floor, this->segmentedSides, number_of_ceiling_floor_surfaces*sizeof(Plane3D));
    
    //printing ceiling and floor
//    std::cout << "\nCeiling and floor walls: \n ";
//    for (int i = 0; i < number_of_ceiling_floor_surfaces; i++){
//        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", walls_ceiling_and_floor[i].corner.x, walls_ceiling_and_floor[i].corner.y, walls_ceiling_and_floor[i].corner.z, walls_ceiling_and_floor[i].S1.x , walls_ceiling_and_floor[i].S1.y, walls_ceiling_and_floor[i].S1.z, walls_ceiling_and_floor[i].S2.x, walls_ceiling_and_floor[i].S2.y, walls_ceiling_and_floor[i].S2.z );
//    }
    
    //copy every other wall side except ceilings and walls to walls_in
    std::memcpy(walls_in, this->segmentedSides + number_of_ceiling_floor_surfaces, number_of_non_ceiling_floor_surfaces * sizeof(Plane3D));

    //create temp array for walls_out, which is all surfaces without ceiling or floor, or lateral walls
    Plane3D *walls_out = new Plane3D[number_of_walls_out_array_size];
    std::cout << " Creating temp array walls_out with size : " <<slices - tilesPerSide * 2 << " \n";
    //create temp array for 4 lateral walls
    Plane3D *lateral_walls = new Plane3D[4];
    
    //split walls in walls_in into lateral walls and walls_out
    //store the number of walls in walls_out
    int number_of_walls_out = splitWalls(walls_in, tilesPerSide, walls_out, number_of_non_ceiling_floor_surfaces, lateral_walls, listener, 1, 1);
    
    std::cout << "\n Number of surfaces in walls_out: " << number_of_walls_out << " \n";
    
    //printing the rest of the walls, except lateral walls
//    std::cout << "\nAll walls except lateral walls: \n ";
//    for (int i = 0; i < number_of_walls_out; i++){
//        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", walls_out[i].corner.x, walls_out[i].corner.y, walls_out[i].corner.z, walls_out[i].S1.x , walls_out[i].S1.y, walls_out[i].S1.z, walls_out[i].S2.x, walls_out[i].S2.y, walls_out[i].S2.z );
//    }
    
    //printing the lateral walls before segmentation
//    std::cout << "\nLateral walls before segmentation: \n ";
//        for (int i = 0; i < 4; i ++){
//                   printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", lateral_walls[i].corner.x, lateral_walls[i].corner.y, lateral_walls[i].corner.z, lateral_walls[i].S1.x , lateral_walls[i].S1.y, lateral_walls[i].S1.z, lateral_walls[i].S2.x, lateral_walls[i].S2.y, lateral_walls[i].S2.z );
//        }
    
    //create array to store segmented lateral walls
    // the maximum number of segmented walls is channels + 4, which means extra +1 at every corner
    Plane3D *segmented_lateral_walls = new Plane3D[channels + 4];
    
    //segment the lateral walls
     int number_of_lateral_walls = segmentWallsBasedOnAzimuth(lateral_walls, listener, segmented_lateral_walls, channels);
    
    std::cout << "\nNumber of lateral walls: " << number_of_lateral_walls << " \n";
    
//    std::cout << "\nSegmented lateral walls: \n ";
//    //printing the segmented lateral walls
//    for (int i = 0; i < number_of_lateral_walls; i ++){
//        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmented_lateral_walls[i].corner.x, segmented_lateral_walls[i].corner.y, segmented_lateral_walls[i].corner.z, segmented_lateral_walls[i].S1.x , segmented_lateral_walls[i].S1.y, segmented_lateral_walls[i].S1.z, segmented_lateral_walls[i].S2.x, segmented_lateral_walls[i].S2.y, segmented_lateral_walls[i].S2.z );
//    }
    
    std::cout << "\nTotal walls intended are (including lateral walls), excluding ceiling and floor: " << number_of_walls_out_array_size << "\n";
    
    std::cout << "\nTotal walls now that we have are, excluding ceiling and floor: " << number_of_lateral_walls + number_of_walls_out << "\n";
    
    //slice randomly walls in walls_out until we have the intended number of surfaces
    int remainder_walls = number_of_walls_out_array_size - number_of_walls_out - number_of_lateral_walls;
    
    assert(remainder_walls >= 0);
    
    std::cout << "\nRemainder walls to make are: " << remainder_walls << "\n";
    
    
    int final_numbers_of_walls_out = sliceRemainder(number_of_walls_out, remainder_walls, walls_out);
    
    //prepare copy of all surface arrays to the global Cuboid variable
    this->segmentedSides = new Plane3D[slices];
    
    //copy ceiling and floor
    memcpy(this->segmentedSides, walls_ceiling_and_floor, number_of_ceiling_floor_surfaces*sizeof(Plane3D));
    
    //copy the rest of the walls except lateral walls
    memcpy(&this->segmentedSides[number_of_ceiling_floor_surfaces], walls_out, final_numbers_of_walls_out * sizeof(Plane3D));
    
    //copy the lateral walls
    memcpy(&this->segmentedSides[number_of_ceiling_floor_surfaces + final_numbers_of_walls_out], segmented_lateral_walls, number_of_lateral_walls * sizeof(Plane3D));
    
    
    assert((number_of_ceiling_floor_surfaces + final_numbers_of_walls_out + number_of_lateral_walls) == slices);

    
    //free memory
    delete [] segmented_lateral_walls;
    delete [] lateral_walls;
    delete [] walls_out;
    delete [] walls_ceiling_and_floor;
    delete [] walls_in;
    


//    //printout all walls
//    std::cout << "\nAll walls : \n ";
//    for (int i = 0; i < slices; i++){
//        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", this->segmentedSides[i].corner.x, this->segmentedSides[i].corner.y, this->segmentedSides[i].corner.z, this->segmentedSides[i].S1.x , this->segmentedSides[i].S1.y, this->segmentedSides[i].S1.z, this->segmentedSides[i].S2.x, this->segmentedSides[i].S2.y, this->segmentedSides[i].S2.z );
//    }
    
    this->elements = slices;

}





/*
 * Segment lateral walls based on listener's azimuth
 *
 *@param lateral_walls      = array containing 4 lateral walls in a rectangular rool
 *@param listener           = listener's location
 *@param segmented_lateral_walls    = output array containing segmented lateral walls based on listener's location
 *@param channels           = listener's azimuthal channels (not to be confused with HRTF channels)
 *
 *@returns                  = number of elements in segmented_lateral_walls
 */
int Cuboid::segmentWallsBasedOnAzimuth(Plane3D* lateral_walls, Vector3D listener, Plane3D* segmented_lateral_walls, int channels)
{
    
    float angle_step = 360.f / (float) channels;
    
    //make front direction vector
    Vector3D azimuth = Vector3D(0,1,0);
    azimuth = azimuth.normalize();
    
    Ray listener_ray = Ray(listener, azimuth);
    
    float *temp_ray_points = new float[channels];
    int segmented_lateral_walls_counter = 0;
    
    for (int i = 0; i<4; i++){
        
        //counting how many intersections per wall
        int temp_ray_points_index = 0;
        //get the ray on the wall
        Ray lateral_wall_ray = getRayFromWallPatch(listener, lateral_walls[i]);
        //set its length, so that it doesn't return out of bounds intersection
        lateral_wall_ray.wall_length = lateral_walls[i].S2.magnitude();
        
        //        std::cout << "\nNEW lateral wall \n" ;
        //        lateral_wall_ray.print_self_ray();
        
        //find all rays that intersect this wall
        for (int j = 0; j<channels ;j++){
            
            //get the azimuth
            float azimuth_angle = (float) j * angle_step;
            //update listener ray
            listener_ray.updateDirectionVector(azimuth_angle);
            
            //get intersection
            bool intersect = lateral_wall_ray.get_intersection(listener_ray);
            
            if (intersect)
            {
                //                std::cout << "Intersection! at angle ";
                //                std::cout << azimuth_angle << " and u is " << lateral_wall_ray.u << " \n ";
                //                listener_ray.print_self_ray();
                //get the scalar u on the lateral wall ray
                temp_ray_points[temp_ray_points_index] = lateral_wall_ray.u;
                temp_ray_points_index ++;
            }
        }
        
        //        std::cout << "\n Lateral wall " << i << " has intersections: " << temp_ray_points_index << "\n";
        //sort the scalar u to get it from left to right
        vDSP_vsort(temp_ray_points, temp_ray_points_index, 1);
        
        
        //start creating segmented lateral walls
        Vector3D corner_before = lateral_walls[i].corner;
        Vector3D corner_after = Vector3D();
        float distance_between_corners = 0.f;
        
        float s2_length = 0;
        
        
        for (int j = 0; j<temp_ray_points_index; j++){
            
            corner_after = lateral_wall_ray.get_vector(temp_ray_points[j]);
            corner_after.z = lateral_walls[i].corner.z;
            
            distance_between_corners = corner_before.distance(corner_after);
            
//            printf("distance between corners: %f \n", distance_between_corners);
            
            if (distance_between_corners > 0.0001){
                //create new segmented wall
                s2_length += distance_between_corners;
                segmented_lateral_walls[segmented_lateral_walls_counter] = Plane3D(corner_before, lateral_walls[i].S1,
                                                                                   lateral_walls[i].S2.scalarMult(distance_between_corners /lateral_walls[i].S2.magnitude()));
                segmented_lateral_walls_counter ++;
            }
            
            corner_before.x = corner_after.x;
            corner_before.y = corner_after.y;
            corner_before.z = corner_after.z;
            
        }
        
        // the last segment
        corner_after = lateral_walls[i].corner.add(lateral_walls[i].S2);
        distance_between_corners = corner_before.distance(corner_after);
        if (distance_between_corners > 0.00001){
            s2_length += distance_between_corners;
            segmented_lateral_walls[segmented_lateral_walls_counter] = Plane3D(corner_before, lateral_walls[i].S1,
                                                                               lateral_walls[i].S2.scalarMult(distance_between_corners /lateral_walls[i].S2.magnitude()));
            segmented_lateral_walls_counter ++;
        }
        
        
        //        printf("S2 length %f lateral walls s2 magnitude %f \n", s2_length, lateral_walls[i].S2.magnitude());
        
        //        std::cout << "s2length " << s2_length << "  lateral walls s2 magnitude " << lateral_walls[i].S2.magnitude();
        
        //make sure that the total length of segmented lateral walls on this wall is the same as the length of the lateral wall before segmentation
        assert( abs (s2_length - lateral_walls[i].S2.magnitude()) <= 0.00001f );
        
    }
    
    //free memory
    delete[] temp_ray_points;
    
    return segmented_lateral_walls_counter;
    
}

/*
 * Get the ray starting from wall's corner at listener height. Wall patch should come from lateral_walls
 */
Ray Cuboid::getRayFromWallPatch(Vector3D L, Plane3D wall_patch){
    
    //get the corner point at listener's height
    Vector3D new_corner = Vector3D(wall_patch.corner.x, wall_patch.corner.y, L.z);
    Vector3D direction = Vector3D(wall_patch.S2.x, wall_patch.S2.y, wall_patch.S2.z).normalize();
    
    return Ray(new_corner, direction);
    
}


/*
 * Split walls into lateral walls and and the rest of the walls
 *
 * walls_in         = original surface patches
 * walls_out        = all walls except lateral walls
 * lateral_walls    = lateral walls with height equal to listener's head length. This always have 4 sides
 *
 * Returns          = the number of walls in walls_out
 */
int Cuboid::splitWalls(Plane3D* walls_in, int walls_per_side, Plane3D* walls_out, int n, Plane3D* lateral_walls, Vector3D listener, float room_width, float room_length){
    
    float listener_height = listener.z;
    float distance_from_listener_height = 0.1f; //20 cm below and above listener
    //assert that the bottom of the head isn't "below" the floor
    assert(listener.z - distance_from_listener_height >= 0);
    Vector3D new_S1 = Vector3D(0.f,0.f,distance_from_listener_height*2);
    
    //create 4 long walls along listener's lateral direction
    lateral_walls[0] = Plane3D(Vector3D(walls_in[0].corner.x, walls_in[0].corner.y, walls_in[0].corner.z + listener_height - distance_from_listener_height), new_S1, Vector3D(room_width, 0, 0));
    lateral_walls[1] = Plane3D(Vector3D(walls_in[0+walls_per_side].corner.x  ,walls_in[0+walls_per_side].corner.y, walls_in[0+walls_per_side].corner.z+ listener_height - distance_from_listener_height), new_S1, Vector3D(0, room_length, 0));
    lateral_walls[2] = Plane3D(Vector3D(walls_in[0+walls_per_side*2].corner.x  ,walls_in[0+walls_per_side*2].corner.y, walls_in[0+walls_per_side*2].corner.z+ listener_height - distance_from_listener_height), new_S1, Vector3D(-1.f*room_width, 0, 0));
    lateral_walls[3] = Plane3D(Vector3D(walls_in[0+walls_per_side*3].corner.x,walls_in[0+walls_per_side*3].corner.y, walls_in[0+walls_per_side*3].corner.z  + listener_height - distance_from_listener_height), new_S1, Vector3D(0, -1.f*room_length, 0));
    
    int total_new_walls = 0;
    //cut out original walls
    for (int i = 0 ; i<n ; i++){
        //        std::cout << "\n Side " << i <<" \n";
        
        //        printf("{{%f, %f, %f},{%f, %f, %f}, {%f, %f, %f}},", walls_in[i].corner.x, walls_in[i].corner.y, walls_in[i].corner.z, walls_in[i].S1.x, walls_in[i].S1.y, walls_in[i].S1.z, walls_in[i].S2.x, walls_in[i].S2.y, walls_in[i].S2.z);
        
        
        float lower_bound_wall = walls_in[i].corner.z;
        float upper_bound_wall = walls_in[i].corner.add(walls_in[i].S1).z;
        
        float lower_bound_listener_wall = listener_height - distance_from_listener_height;
        float upper_bound_listener_wall = listener_height + distance_from_listener_height;
        
        //        std::cout << "lower bound wall " << lower_bound_wall << " upper bound wall " << upper_bound_wall
        //        << " lower_bound_listener_wall " << lower_bound_listener_wall
        //        << " upper_bound_listener_wall " << upper_bound_listener_wall;
        
        //check if within listener
        if ( !(upper_bound_wall < lower_bound_listener_wall && lower_bound_wall < lower_bound_listener_wall) &&
            !(upper_bound_wall > upper_bound_listener_wall && lower_bound_wall > upper_bound_listener_wall)
            )
        {
            //            std::cout << " Intersects listener head-plane \n" ;
            // check if entirely within, if yes, then skip
            if (lower_bound_wall >= lower_bound_listener_wall && upper_bound_wall <= upper_bound_listener_wall){
                //                std::cout << " Wall within \n" ;
                continue; // dont put this on walls out
            }
            
            // otherwise cut it shorter, store on walls out
            else{
                // if in between, split into two surfaces
                if (lower_bound_wall < lower_bound_listener_wall && upper_bound_wall > upper_bound_listener_wall)
                {
                    //                    std::cout << " Wall in between \n" ;
                    // the below portion
                    //find leftover height of that surface
                    float h_below = (listener_height - distance_from_listener_height) - walls_in[i].corner.z;
                    assert(h_below > 0.001);
                    Plane3D new_plane_below = Plane3D(walls_in[i].corner, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y, h_below) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane_below;
                    total_new_walls++;
                    
                    // the above portion
                    //find the leftover height of that surface
                    float h_above = walls_in[i].corner.add(walls_in[i].S1).z - (listener_height + distance_from_listener_height);
                    Vector3D corner_above = Vector3D(walls_in[i].corner.x, walls_in[i].corner.y, walls_in[i].corner.z + h_below + 2*distance_from_listener_height);
                    assert(h_above > 0.001);
                    Plane3D new_plane_above = Plane3D(corner_above, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y, h_above) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane_above;
                    total_new_walls++;
                }
                
                // if below listener, cut top of S1 shorter
                else if (upper_bound_wall <= upper_bound_listener_wall){
                    
                    //                    std::cout << " Wall below \n" ;
                    //find leftover height of that surface
                    float h = (listener_height - distance_from_listener_height) - walls_in[i].corner.z;
                    //ensure h is positive
                    assert(h > 0.001);
                    
                    Plane3D new_plane = Plane3D(walls_in[i].corner, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y, h) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane;
                    total_new_walls++;
                }
                
                // if above, cut bottom of S1 shorter
                else if (lower_bound_wall >= lower_bound_listener_wall){
                    
                    //                    std::cout << " Wall above \n" ;
                    float h = walls_in[i].S1.z - (listener_height + distance_from_listener_height - walls_in[i].corner.z);
                    assert(h > 0.001);
                    
                    Vector3D corner_above = Vector3D(walls_in[i].corner.x, walls_in[i].corner.y, listener_height + distance_from_listener_height);
                    Plane3D new_plane = Plane3D(corner_above, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y,h) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane;
                    total_new_walls++;
                }
                
                else{
                    //                    std::cout << " \n This should never be printed \n" ;
                }
            }
        }
        else{
            //            std::cout << " Storing original wall \n" ;
            //             printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", walls_in[i].corner.x, walls_in[i].corner.y, walls_in[i].corner.z, walls_in[i].S1.x , walls_in[i].S1.y, walls_in[i].S1.z, walls_in[i].S2.x, walls_in[i].S2.y, walls_in[i].S2.z );
            Plane3D new_plane = Plane3D(walls_in[i].corner, walls_in[i].S1 ,walls_in[i].S2);
            walls_out[total_new_walls] = new_plane;
            total_new_walls++;
        }
    }
    
    return total_new_walls;
    
}



/*Slices random remaining planes until we have 'slices' in total
 *
 *int last_index                = the last index of surfaces in array walls_out
 *int remainder                 = the number of surfaces we need to make
 *Plane3D* newSegmentedSides    = the planes
 */
int Cuboid::sliceRemainder(int last_index, int remainder, Plane3D* newSegmentedSides){

    printf("Supposed number of surfaces : %i, now have to make %i more. \n", last_index + remainder, remainder);
    
    int original_wall_last_index = last_index;
    int index = last_index; //last index, mutable
    
    std::cout << " last index : " << index << "\n" ;
    
    srand(11);
    //            srand(time(NULL));
    int randNum = rand()%(original_wall_last_index-1);
    
    //divide some plane into two
    for (int i = 0; i<remainder; i++){
        Plane3D refPlane = newSegmentedSides[randNum];
        
        //divide into two along S1
        if (randNum %2 == 0){
            
            Vector3D newCorner = refPlane.corner.add(refPlane.S1.scalarMult(0.5f));
            Vector3D newS1 = refPlane.S1.scalarMult(0.5f);

            newSegmentedSides[randNum] = Plane3D(refPlane.corner, newS1, refPlane.S2);
            newSegmentedSides[index] = Plane3D(newCorner, newS1, refPlane.S2);
            
            index++;
            randNum = rand()%(original_wall_last_index-1);
        }
        
        else{
            Vector3D newCorner = refPlane.corner.add(refPlane.S2.scalarMult(0.5f));
            Vector3D newS2 = refPlane.S2.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner,  refPlane.S1, newS2);
            newSegmentedSides[index] = Plane3D(newCorner,refPlane.S1, newS2);

            index++;
            randNum = rand()%(original_wall_last_index-1);
        }
    }
    
    return index;
    
}


//Even subdivision, then slice the rest randomly
void Cuboid::sliceCube(int slices){
    int toSlice = 0;
    
    while(true){
        if (6*toSlice * toSlice < slices){
            toSlice += 1;
        }
        if (6*toSlice * toSlice > slices){
            toSlice -= 1;
            break;
        }
        
        }
    
    segmentCube(toSlice*toSlice);
    
    int cubicTiles = 6*toSlice*toSlice;
    Plane3D* newSegmentedSides = new Plane3D[slices];
    
    //copy into newSegmentedSides
    for (int i = 0; i<cubicTiles; i++){
        newSegmentedSides[i] = Plane3D(segmentedSides[i].corner, segmentedSides[i].S1, segmentedSides[i].S2);
    }
    
    this->elements = slices;
    int remainder = slices - cubicTiles;
    int index = cubicTiles;
    
    srand(11);
//            srand(time(NULL));
    
    int randNum = rand()%(cubicTiles-1);
    //divide some plane into two
    for (int i = 0; i<remainder; i++){
        Plane3D refPlane = segmentedSides[randNum];
        //divide into two along S1
        if (randNum %2 == 0){
            Vector3D newCorner = refPlane.corner.add(refPlane.S1.scalarMult(0.5f));
            Vector3D newS1 = refPlane.S1.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner, newS1, refPlane.S2);
            newSegmentedSides[index] = Plane3D(newCorner, newS1, refPlane.S2);
            index++;
            randNum = rand()%(cubicTiles-1);
        }
        else{
            Vector3D newCorner = refPlane.corner.add(refPlane.S2.scalarMult(0.5f));
            Vector3D newS2 = refPlane.S2.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner,  refPlane.S1, newS2);
            newSegmentedSides[index] = Plane3D(newCorner,refPlane.S1, newS2);
            index++;
            randNum = rand()%(cubicTiles-1);
        }
    }
    
    segmentedSides = new Plane3D[slices];
    //copy back into segmentedsides
    for (int i = 0; i<elements; i++){
        segmentedSides[i] = Plane3D(newSegmentedSides[i].corner, newSegmentedSides[i].S1, newSegmentedSides[i].S2);
    }
    
    ////
        for (int i = 0; i<(slices); i++){
            printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x , segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z );
        }
    
    delete [] newSegmentedSides;
    
//    free(newSegmentedSides);
    
////    
//    for (int i = 0; i<(slices); i++){
//        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x , segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z );
//    }
}



//Even Segmentations
int Cuboid::segmentCube(int tilesPerSide){
    
    //Make sure tilesPerSide can be exactly square-rooted
    try
    {
        if(sqrtf(tilesPerSide) != floorf(sqrtf(tilesPerSide))){
            throw 1;
    }
    }
    catch (int e){
        printf("Please enter some number that can be square rooted for tilesPerSide \n");
        exit(1);
    }
    
    segmentedSides = new Plane3D[tilesPerSide*6];
    
    //now divide each side into tiles per side
    dimensions = static_cast<int>(sqrtf(tilesPerSide));
    this->tilesPerSide = tilesPerSide;
    
    int index = 0;
    for (int k = 0; k<6; k++){
        
        //get the plane
        Plane3D plane = sides[k];
        
        Vector3D s1 = plane.S1.scalarMult((float)1/dimensions);
        Vector3D s2 = plane.S2.scalarMult((float)1/dimensions);
        
        for(int i = 0; i<dimensions; i++){
            for (int j = 0; j<dimensions; j++){
                //Get the corner
                Vector3D c = plane.corner.add(plane.S1.scalarMult((float)j/dimensions));
                c = c.add(plane.S2.scalarMult((float)i/dimensions));
                
                segmentedSides[index] = Plane3D(c,s1,s2);
                segmentedSides[index].normal = plane.normal;
                index = index + 1;
            }
        }
    }
    

    elements = 6*tilesPerSide;
//    return index;
    
//    Printing the result
//    for (int i = 0; i< 6*tilesPerSide ; i++){
//        printf("{{%f, %f, %f},{%f, %f, %f},{%f, %f, %f}},", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x, segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z);
//    }
    return index;
    
}


float Cuboid::segmentCubeOnce(){
    //segment floor and ceiling
    int index = 0;

    for (int k = 0; k<6; k++){
        
        //get the plane
        Plane3D plane = sides[k];
        
        segmentedSides[index] = plane;
        index ++;
    }

    return index;
    
    
}


void Cuboid::segmentCubeBasedOnProjectedArea(int numDelays, Vector3D S, Vector3D L){
    segmentedSides = new Plane3D[numDelays];

    elements = numDelays;
    
//    int index = segmentCubeOnce();
    int index = segmentCube(9);
    int delaysCeilingFloor = 0;
    
    while(index<numDelays){
        
        //get the side with the nearest distance to listener
        int maxIndex = 0;
        float maxArea = 100000000;
        
        for (int i = delaysCeilingFloor; i<index; i++){

            float comparisonValue = segmentedSides[i].getMidpoint().distance(L); //compare with the nearest ear's azimuth
            // only segment the walls not the floor

            if (comparisonValue < maxArea && segmentedSides[i].subdivided < 4 &&  segmentedSides[i].getMidpoint().z != 0 && segmentedSides[i].getMidpoint().z != zLength){
                printf("%f %f %f wallmidpt %f %f %f L \n", segmentedSides[i].getMidpoint().x, segmentedSides[i].getMidpoint().y, segmentedSides[i].getMidpoint().z, L.x, L.y, L.z);
                maxArea = comparisonValue;
                maxIndex = i;
            }
//            if (comparisonValue < maxArea && segmentedSides[i].subdivided < 7 && (segmentedSides[i].getMidpoint().y - L.y) > 0 && segmentedSides[i].getMidpoint().z != 0 && segmentedSides[i].getMidpoint().z < 2.9){
//                printf("%f %f %f wallmidpt %f %f %f L \n", segmentedSides[i].getMidpoint().x, segmentedSides[i].getMidpoint().y, segmentedSides[i].getMidpoint().z, L.x, L.y, L.z);
//                maxArea = comparisonValue;
//                maxIndex = i;
//            }

        }
        
        int randNum = rand()%100;
        //divide some plane into two

            //divide into two along S1
            if (randNum %2 == 0){
                Vector3D newCorner = segmentedSides[maxIndex].corner.add(segmentedSides[maxIndex].S1.scalarMult(0.5f));
                Vector3D newS1 = segmentedSides[maxIndex].S1.scalarMult(0.5f);
                int oldIndex = segmentedSides[maxIndex].subdivided;
                segmentedSides[maxIndex] = Plane3D(segmentedSides[maxIndex].corner, newS1, segmentedSides[maxIndex].S2);
                segmentedSides[maxIndex].subdivided = oldIndex + 1;
                segmentedSides[index] = Plane3D(newCorner, newS1, segmentedSides[maxIndex].S2);
                segmentedSides[index].subdivided = segmentedSides[maxIndex].subdivided;
                index++;
            }
            else{
                Vector3D newCorner = segmentedSides[maxIndex].corner.add(segmentedSides[maxIndex].S2.scalarMult(0.5f));
                Vector3D newS2 = segmentedSides[maxIndex].S2.scalarMult(0.5f);
                int oldIndex = segmentedSides[maxIndex].subdivided;
                segmentedSides[maxIndex] = Plane3D(segmentedSides[maxIndex].corner,  segmentedSides[maxIndex].S1, newS2);
                segmentedSides[maxIndex].subdivided = oldIndex + 1;
                segmentedSides[index] = Plane3D(newCorner,segmentedSides[maxIndex].S1, newS2);
                segmentedSides[index].subdivided = segmentedSides[maxIndex].subdivided;
                index++;
            }
        
        
        //split that patch
        index = dividePlane(segmentedSides[maxIndex], index, maxIndex, S, L);
        
        
    }
    
    for (int i = 0; i<(index); i++){
        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x , segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z );
    }

}


//int Cuboid::dividePlaneAlongS1(Plane3D divide, int index, int sourceIndex){
//    
//    Vector3D newCorner = divide.corner.add(divide.S2.scalarMult(0.5));
//    Vector3D newS2 = divide.S2.scalarMult(0.5);
//    segmentedSides[sourceIndex] = Plane3D(divide.corner, divide.S1, newS2);
//    
//    segmentedSides[index] = Plane3D(newCorner, divide.S1, newS2);
//    index++;
//    
//    return index;
//}

int Cuboid::dividePlane(Plane3D divide, int index, int sourceIndex, Vector3D S, Vector3D L ){
//    
    Vector3D omegaS1L= L.subtract(divide.corner.add(divide.S1.scalarMult(0.5))).normalize();
    Vector3D omegaS1S= S.subtract(divide.corner.add(divide.S1.scalarMult(0.5))).normalize();
    float projectedLengthS1 = divide.S1.magnitude() * omegaS1L.dotProduct(divide.normal) + divide.S1.magnitude() * omegaS1S.dotProduct(divide.normal);
    
    Vector3D omegaS2L = L.subtract(divide.corner.add(divide.S2.scalarMult(0.5))).normalize();
    Vector3D omegaS2S = S.subtract(divide.corner.add(divide.S2.scalarMult(0.5))).normalize();
    float projectedLengthS2 = divide.S2.magnitude() * omegaS2L.dotProduct(divide.normal) + divide.S2.magnitude() * omegaS2S.dotProduct(divide.normal);
 
    Vector3D c2 = divide.corner.add(divide.S1.scalarMult(0.5));
    Vector3D s1 = divide.S1.scalarMult(0.5);
    Vector3D c22 = divide.corner.add(divide.S2.scalarMult(0.5));
    Vector3D s2 = divide.S2.scalarMult(0.5);


    if (projectedLengthS1 > projectedLengthS2){
        //divide along S1
        int subdivision = segmentedSides[sourceIndex].subdivided;
        segmentedSides[sourceIndex] = Plane3D(divide.corner, s1, divide.S2);
        segmentedSides[sourceIndex].subdivided = subdivision + 1;

        segmentedSides[index] = Plane3D(c2, s1, divide.S2);
        segmentedSides[index].subdivided = segmentedSides[sourceIndex].subdivided;
        
        index++;
    }
    else{
        //Divide along S2
        int subdivision = segmentedSides[sourceIndex].subdivided;
        segmentedSides[sourceIndex] = Plane3D(divide.corner, divide.S1, s2);
        segmentedSides[sourceIndex].subdivided = subdivision + 1;

        segmentedSides[index] = Plane3D(c22, divide.S1, s2);
        segmentedSides[index].subdivided = segmentedSides[sourceIndex].subdivided;
        index++;
        

    }
    return index;
}

////Either S1 or S2
//bool Cuboid::longestDimension(Plane3D patch){
//    if (patch.S1.magnitude() > patch.S2.magnitude()){
//        return true;
//    }
//    else{
//        return false;
//    }
//}


void Cuboid::getDelayValues(int *delayValues, Vector3D LLE, Vector3D LRE, Vector3D S, int Hz){
    for (int i =0; i< elements; i++){
        Vector3D p = segmentedSides[i].getMidpoint();
        float d1 = S.subtract(p).magnitude();
        float d2 = LLE.subtract(p).magnitude();
        float d2R = LRE.subtract(p).magnitude();
        
        //set delay times to the nearest ear
        if (d2R < d2){
            d2 = d2R;
        }
        
        delayValues[i] = static_cast<int>((d1+d2)/SOUNDSPEED*Hz);
    }
}




float Cuboid::projAreaSubSec(Plane3D r, Vector3D L){
    // centre of r
    Vector3D c = r.getMidpoint();
    
    // area of r
    float A = r.getArea();
    
    // surface normal of r
    Vector3D n =   r.S1.crossProduct(r.S2).normalize();//  normalize(crossProduct(r.s1, r.s2));
    
    // L-c
    Vector3D cToL = L.subtract(c);
    
    // distance from L to centre of r
    float d = cToL.magnitude();
    
    // direction from c to L
    Vector3D Omega = cToL.normalize();
    
    // projected area = Area (n . Omega) / d^2
    return (A * fabs(n.dotProduct(Omega))) / (d*d);
}




/*
 * Estimate the area of the rectangle r projected onto the unit sphere
 * centred at L.
 *
 * @param r         the rectangle
 * @param L         point at centre of the unit sphere of projection
 * @param divisions number of times to subdivide the rectangle in each
 *                  direction (l x w). higher values are more accurate.
 */
float Cuboid::ProjectedArea_rectangleSubDiv(Plane3D r, Vector3D L, size_t divisions){
    // how many sub patches?
    size_t numPatches = divisions*divisions;
    
    // allocate memory
    Plane3D* subMesh = new Plane3D[numPatches];
    
    // subdivide r
    Vector3D s1s = r.S1.scalarMult(1.0f/(float)divisions);//  scalarDivide(r.s1,divisions);
    Vector3D s2s = r.S2.scalarMult(1.0f/(float)divisions);;
    for(size_t i=0; i<divisions; i++){
        for(size_t j=0; j<divisions; j++){
            Vector3D cij =  r.corner.add(s1s.scalarMult(i)).add(s2s.scalarMult(j));// add3(r.c,
                             //  scalarProduct(i,s1s),
                             //  scalarProduct(j,s2s));
            Plane3D rij = Plane3D(cij, s1s, s2s);
            subMesh[divisions*i + j] = rij;
        }
    }
    
    // get projected area of each subdivision
    float projArea = 0.0f;
    for(size_t i=0; i<numPatches; i++)
        projArea += projAreaSubSec(subMesh[i],L);
    
    //
    free(subMesh);
    
    return projArea;
}



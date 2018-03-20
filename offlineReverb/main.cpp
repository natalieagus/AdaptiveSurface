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
//    
//    clock_t end = clock();
//    double elapsed_msecs = double(end - begin) / CLOCKS_PER_SEC * 1000.f;
//    
//    printf("Time elapsed: %f ms\n", elapsed_msecs);
}


class Ray{
    
public:
    
    Ray(){
        this-> p = Vector3D();
        this-> d = Vector3D();
        this-> u = 0;
    }
    
    Ray(Vector3D point, Vector3D direction)
    {
        this-> p = point;
        this-> d = direction.normalize();
        this-> u = 0;
    };
    
    Ray(Vector3D point, Vector3D direction, float wall_length)
    {
        this-> p = point;
        this-> d = direction.normalize();
        this-> u = 0;
        this-> wall_length = wall_length;
    };
    
    
    void updateDirectionVector(float theta_azimuth)
    {
        if (theta_azimuth > 360.f){
            theta_azimuth = remainder(theta_azimuth, 360.f);
        }
        
        theta_azimuth = theta_azimuth / 180.f * M_PI;
        
        this->d = Vector3D(sin(theta_azimuth), cos(theta_azimuth), d.z);
        this->d = d.normalize();
        this->u = 0.f;
    };
    
    void print_self_ray(){
        printf("p:{%f %f %f}, d:{%f %f %f}, u:{%f} \n ", this->p.x, this->p.y, this->p.z, this->d.x, this->d.y, this->d.z, this->u);
    }
    
    void print_vector(Vector3D in){
        printf("{%f, %f, %f}, ", in.x, in.y, in.z);
    }
    
    void print_self(){
        print_vector(get_vector());
    }
    
    Vector3D get_vector(){
        return this->p.add(this->d.scalarMult(this->u));
    }
    
    Vector3D get_vector(float u){
        return this->p.add(this->d.scalarMult(u));
    }
    
    bool get_intersection(Ray x){
        float dx = x.p.x - this->p.x;
//        std::cout << dx << " dx \n";
        float dy = x.p.y - this->p.y;
//        std::cout << dy << " dy \n";
    
        float D = x.d.x * this->d.y - x.d.y * this->d.x;
//        std::cout << D << " D \n";
        
        //rays are not parallel
        if (fabs(D) > 0.00001){
            float intersection_scalar_this = (dy * x.d.x - dx * x.d.y)/D;
            float intersection_scalar_x = (dy * this->d.x - dx * this->d.y)/D;
            
//            printf("this ray's scalar %f, other ray scalar %f \n", intersection_scalar_this, intersection_scalar_x);
            
            if (intersection_scalar_this >= 0 && intersection_scalar_x >= 0 &&
                ((this->wall_length > 0 && intersection_scalar_this <= wall_length) ||
                 (this->wall_length < 0))
                )
            {
                if (abs(intersection_scalar_this) <= 0.00001 ) intersection_scalar_this = 0.f;
                
                this->u = intersection_scalar_this;
//                return intersection_scalar;
                return true;
                
            }
            
            // Negative intersection means Ray x is behind this ray
            else return false;
            
        //this will not set anything, but this is the formula for
        //the scalar in the other ray x
//        x.u = (dy * this->d.x - dx * this->d.y)/D;
        
        }
        
        //parallel rays, they do not intersect
        else{
            return false;
        }
    }

    
    Vector3D p;
    Vector3D d;
    float u;
    float wall_length = -1;
    
    
};

/*
 * Get the ray starting from wall's corner at listener height. Wall patch should come from lateral_walls
 */
Ray getRayFromWallPatch(Vector3D L, Plane3D wall_patch){
    
    //get the corner point at listener's height
    Vector3D new_corner = Vector3D(wall_patch.corner.x, wall_patch.corner.y, L.z);
    Vector3D direction = Vector3D(wall_patch.S2.x, wall_patch.S2.y, wall_patch.S2.z).normalize();
    
    return Ray(new_corner, direction);
    
}

////only works of wallpatch is in the form of (corner, S1 pointing +ve z, and S2)
//bool checkRayFromWallPatch(Vector3D L, Plane3D wall_patch){
//
//
//    if ((wall_patch.corner.z <= L.z ) && (L.z <= wall_patch.corner.z + wall_patch.S1.z)){
//        return true;
//    }
//
//    else return false;
//}


/*
 * Split walls into lateral walls and and the rest of the walls
 */
//walls_in are side walls in rectangular room
//ensure size of walls_out are 2*n
int splitWalls(Plane3D* walls_in, int walls_per_side, Plane3D* walls_out, int n, Plane3D* lateral_walls, Vector3D listener, float room_width, float room_length){
    
    float listener_height = listener.z;
    float distance_from_listener_height = 0.1f; //20 cm below and above listener
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
                if (lower_bound_wall <= lower_bound_listener_wall && upper_bound_wall >= upper_bound_listener_wall)
                {
//                    std::cout << " Wall in between \n" ;
                    // the below portion
                    //find leftover height of that surface
                    float h_below = (listener_height - distance_from_listener_height) - walls_in[i].corner.z;
                    Plane3D new_plane_below = Plane3D(walls_in[i].corner, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y, h_below) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane_below;
                    total_new_walls++;
                    
                    // the above portion
                    //find the leftover height of that surface
                    float h_above = walls_in[i].corner.add(walls_in[i].S1).z - (listener_height + distance_from_listener_height);
                    Vector3D corner_above = Vector3D(walls_in[i].corner.x, walls_in[i].corner.y, walls_in[i].corner.z + h_below + 2*distance_from_listener_height);
                     Plane3D new_plane_above = Plane3D(corner_above, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y, h_above) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane_above;
                    total_new_walls++;
                }
                
                // if below listener, cut top of S1 shorter
                else if (upper_bound_wall <= upper_bound_listener_wall){

//                    std::cout << " Wall below \n" ;
                    //find leftover height of that surface
                    float h = (listener_height - distance_from_listener_height) - walls_in[i].corner.z;
                    
                    Plane3D new_plane = Plane3D(walls_in[i].corner, Vector3D(walls_in[i].S1.x, walls_in[i].S1.y, h) ,walls_in[i].S2);
                    walls_out[total_new_walls] = new_plane;
                    total_new_walls++;
                }
                
                // if above, cut bottom of S1 shorter
                else if (lower_bound_wall >= lower_bound_listener_wall){

//                    std::cout << " Wall above \n" ;
                    float h = walls_in[i].S1.z - (listener_height + distance_from_listener_height - walls_in[i].corner.z);
                    
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

/*
 * Segment lateral walls based on listener's azimuth
 */
int segmentWallsBasedOnAzimuth(Plane3D* lateral_walls, Vector3D listener, Plane3D* segmented_lateral_walls, int channels)
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

            
            if (distance_between_corners > 0.00001){
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
                                          
    return segmented_lateral_walls_counter;
    
}



int main(int argc, char* argv[])
{
//    float impulseLength = 5.0f;
//
//    std::ofstream impulseLeft;
//    std::ofstream impulseRight;
//    std::ifstream inFile;
//
//    saveImpulse(16, FS*impulseLength, &impulseLeft, &impulseRight);
//
//    std::cout << "\ndone.\n";
    
    
//    Vector3D listener = Vector3D(0,0,1);
//    Vector3D corner = Vector3D(-1,1,0);
//    Vector3D corner_direction = Vector3D(1,0,0);
//
//    //make front direction vector
//    Vector3D azimuth = Vector3D(0,1,0);
//    azimuth = azimuth.normalize();
//
//    Ray listener_ray = Ray(listener, azimuth);
//
//    Ray wall_ray = Ray(corner, corner_direction);
//
////    listener_ray.print_vector(listener_ray.get_vector());
//
//    bool u = listener_ray.get_intersection(wall_ray);
//    bool w = wall_ray.get_intersection(listener_ray);
//
//    std::cout << u << " bool u ";
//    std::cout << w << " bool w ";
//
////    listener_ray.print_vector(listener_ray.get_vector(u));
////    listener_ray.print_self();
////    wall_ray.print_vector(wall_ray.get_vector(w));
//    wall_ray.print_self_ray();
//    printf("\n");
//
//    listener_ray.updateDirectionVector(-90.f);
//    listener_ray.print_self_ray();
////    listener_ray.print_vector(listener_ray.get_vector(1));
//
//    u = listener_ray.get_intersection(wall_ray);
//    listener_ray.print_self_ray();
//
//    std::cout << u << " \n ";
    
    
    Vector3D listener = Vector3D(0.5,0.5,0.5);
    
//    //make front direction vector
//    Vector3D azimuth = Vector3D(0,1,0);
//    azimuth = azimuth.normalize();
//
//    Ray listener_ray = Ray(listener, azimuth);
//
//    Plane3D walls[4] = {Plane3D(Vector3D(2,2,0),Vector3D(0,0,1),Vector3D(-1,0,0)),Plane3D(Vector3D(2,1,0),Vector3D(0,0,1),Vector3D(-1,0,0)),
//        Plane3D(Vector3D(2,2,1),Vector3D(0,0,1),Vector3D(-1,0,0)), Plane3D(Vector3D(2,1,1),Vector3D(0,0,1),Vector3D(-1,0,0)) };
//
    int tilesPerSide = 9;
    Cuboid cube = Cuboid(1, 1, 1);
    cube.segmentCube(tilesPerSide);

//        for (int i = tilesPerSide*2; i< 6*tilesPerSide ; i++){
//            printf("{{%f, %f, %f},{%f, %f, %f}, {%f, %f, %f}},", cube.segmentedSides[i].corner.x, cube.segmentedSides[i].corner.y, cube.segmentedSides[i].corner.z, cube.segmentedSides[i].S1.x, cube.segmentedSides[i].S1.y, cube.segmentedSides[i].S1.z, cube.segmentedSides[i].S2.x, cube.segmentedSides[i].S2.y, cube.segmentedSides[i].S2.z);
//        }
    
//    Plane3D walls_in[4] =
//    {
//    Plane3D(Vector3D(0,0,0),Vector3D(0,0,1),Vector3D(1,0,0)),
//    Plane3D(Vector3D(1,0,0),Vector3D(0,0,1),Vector3D(0,1,0)),
//    Plane3D(Vector3D(1,1,0),Vector3D(0,0,1),Vector3D(-1,0,0)),
//    Plane3D(Vector3D(0,1,0),Vector3D(0,0,1),Vector3D(0,-1,0))
//    };
    
    Plane3D walls_in[6*tilesPerSide - 2*tilesPerSide];
    int index = 0;
    
    //printing the ceiling and floor
    std::cout << "\nCeilings and floors: \n ";
    for (int i = 0; i < tilesPerSide*2; i++){
        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", cube.segmentedSides[i].corner.x, cube.segmentedSides[i].corner.y, cube.segmentedSides[i].corner.z, cube.segmentedSides[i].S1.x , cube.segmentedSides[i].S1.y, cube.segmentedSides[i].S1.z, cube.segmentedSides[i].S2.x, cube.segmentedSides[i].S2.y, cube.segmentedSides[i].S2.z );
    }
    
    
    for (int i = tilesPerSide*2; i< 6*tilesPerSide ; i++){
        walls_in[index] = cube.segmentedSides[i];
        index ++;
    }
    
    
//    bool out = checkRayFromWallPatch(listener, walls[0]);
    
//    if (out) {
////        Ray wall_ray = getRayFromWallPatch(listener, walls[0]);
////        wall_ray.print_self_ray();
//
//    }
   
//    std::cout << out << "\n";
    
    Plane3D walls_out[tilesPerSide*(6+2)];
    Plane3D lateral_walls[4];
    
    int number_of_remainder_walls = splitWalls(walls_in, tilesPerSide, walls_out, 4*tilesPerSide, lateral_walls, listener, 1, 1);
    
    //printing the rest of the walls, except lateral walls
    std::cout << "\nAll walls except lateral walls: \n ";
    for (int i = 0; i < number_of_remainder_walls; i++){
        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", walls_out[i].corner.x, walls_out[i].corner.y, walls_out[i].corner.z, walls_out[i].S1.x , walls_out[i].S1.y, walls_out[i].S1.z, walls_out[i].S2.x, walls_out[i].S2.y, walls_out[i].S2.z );
    }

    //printing the lateral walls before segmentation
//    for (int i = 0; i < 4; i ++){
//               printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", lateral_walls[i].corner.x, lateral_walls[i].corner.y, lateral_walls[i].corner.z, lateral_walls[i].S1.x , lateral_walls[i].S1.y, lateral_walls[i].S1.z, lateral_walls[i].S2.x, lateral_walls[i].S2.y, lateral_walls[i].S2.z );
//    }
    
    std::cout << "\n number of remainder walls: " << number_of_remainder_walls << " \n";
    
    // the maximum number of segmented walls is channels + 4, which means extra +1 at every corner
    int channels = 16;
    Plane3D* segmented_lateral_walls = new Plane3D[channels + 4];
    
    int number_of_lateral_walls = segmentWallsBasedOnAzimuth(lateral_walls, listener, segmented_lateral_walls, channels);
    
    std::cout << "\nNumber of lateral walls: " << number_of_lateral_walls << " \n";
    
    std::cout << "\nSegmented lateral walls: \n ";
    //printing the segmented lateral walls
    for (int i = 0; i < number_of_lateral_walls; i ++){
        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmented_lateral_walls[i].corner.x, segmented_lateral_walls[i].corner.y, segmented_lateral_walls[i].corner.z, segmented_lateral_walls[i].S1.x , segmented_lateral_walls[i].S1.y, segmented_lateral_walls[i].S1.z, segmented_lateral_walls[i].S2.x, segmented_lateral_walls[i].S2.y, segmented_lateral_walls[i].S2.z );
    }
    
    std::cout << "\nTotal walls intended are: " << tilesPerSide * 6 << "\n";
    
    std::cout << "\nTotal walls now are: " << number_of_lateral_walls + number_of_remainder_walls << "\n";
    return 0;
    
    
}





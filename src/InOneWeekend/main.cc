#include "vec3.h" 
#include "ray.h"
#include "color.h"
#include <iostream> 
#include <algorithm>

// checks whether a ray intersects the sphere at one or more points
bool hit_sphere(const point3& center, double radius, const ray& r) {
    // ray : Q+td
    vec3 oc = center-r.origin();
    vec3 d = r.direction();
    double a = d.length()*d.length();
    double h = dot(d, oc);      // put h=-b/2
    double c = oc.length()*oc.length()-radius*radius;
    auto discriminant = h*h-a*c;
    if(discriminant<0) { 
        return -1; 
    } 
    else { 
        return (h-std::sqrt(discriminant))/a;      // returns the point of intersection in terms of parameter t
    }
}

color ray_color(const ray& r) {
    // currently this doesnt differentiate between objects in front of camera and objects behind it
    auto t=hit_sphere(point3(0,0,-1),0.5,r); 
    if(t>0) { 
        vec3 N=unit_vector(r.at(t)-vec3(0,0,-1));
        return 0.5*color(N.x()+1,N.y()+1,N.z()+1);  // color depends on the components of the normal vector 
    }
    // gradient starting from white and ending at a shade of blue (rgb 0.5,0.7,1)
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y()+1.0);
    return (1-a)*color(1.0, 1.0, 1.0)+a*color(0.5, 0.7, 1.0);
}

int main() {
    // Image 
    auto aspect_ratio = 16.0/9.0;
    int image_width = 400;
    // calculate image height, and ensure it's atleast 1 
    int image_height = std::max(int(image_width/aspect_ratio), 1);

    // Camera 
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height*((double)image_width/image_height);
    auto camera_center = point3(0, 0, 0);

    // calculate vectors across the horizontal and vertical viewport edges 
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // calculate the horizontal and vertical delta vectors from pixel to pixel 
    auto pixel_delta_u = viewport_u/image_width;
    auto pixel_delta_v = viewport_v/image_height;

    // calculate location of upper left pixel 
    auto viewport_upper_left = camera_center-vec3(0, 0, focal_length)-viewport_u/2-viewport_v/2;
    auto pixel00_loc = viewport_upper_left+0.5*(pixel_delta_u+pixel_delta_v);

    // Render
    std::cout<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";
    for (int j = 0; j<image_height; j++) {
        // log progress
        std::clog<<"\rScanlines remaining: "<<image_height-j<<' '<<std::flush;
        for (int i = 0; i<image_width; i++) {
            auto pixel_center = pixel00_loc+(i*pixel_delta_u)+(j*pixel_delta_v);
            auto ray_direction = pixel_center-camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog<<"\rDone.       \n";
}
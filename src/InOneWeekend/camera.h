#ifndef CAMERA_H 
#define CAMERA_H 

#include "hittable.h" 
#include "material.h"
#include "rtweekend.h"

class camera {
    public:
    // Image 
    double aspect_ratio = 1.0;             // ratio of image width over height
    int    image_width = 100;              // rendered image width in pixels 
    int    samples_per_pixel = 10;         // no of random samples for each pixel
    int    max_depth=10;                   // max no of ray bounces into scene

    double vfov = 90;                      // vertical field of view angle, in degrees
    point3 lookfrom = point3(0,0,0);       // point camera is looking from     
    point3 lookat = point3(0,0,-1);        // point camera is looking at  
    vec3   vup = vec3(0,1,0);              // camera-relative "up" direction 

    double defocus_angle = 0;              // variation angle of rays through each pixel 
    double focus_distance = 10;            // distance of camera lookfrom point to plane of focus
    
    void render(const hittable& world) {
        initialize(); 
        // Render
        std::cout<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";
        for (int j = 0; j<image_height; j++) {
            // log progress
            std::clog<<"\rScanlines remaining: "<<image_height-j<<' '<<std::flush;
            for (int i = 0; i<image_width; i++) {
                color pixel_color(0,0,0); 
                for(int sample=0;sample<samples_per_pixel;sample++) { 
                    ray r=get_ray(i,j); 
                    pixel_color+=ray_color(r,max_depth,world); 
                }
                write_color(std::cout, pixel_samples_scale*pixel_color);
            }
        }
        std::clog<<"\rDone.       \n";
    }

    private:
    int    image_height;            // Rendered image height 
    double pixel_samples_scale;     // Color scale factor for a sum of pixel samples
    point3 center;                  // Camera center
    point3 pixel00_loc;             // Location of pixel 0, 0
    vec3   pixel_delta_u;           // Offset to pixel to the right
    vec3   pixel_delta_v;           // Offset to pixel below  
    vec3   u,v,w;                   // Camera frame basis vectors
    vec3   defocus_disk_u;          // Defocus disk horizontal radius
    vec3   defocus_disk_v;          // Defocus disk vertical radius

    void initialize() {
        // calculate image height, and ensure it's atleast 1 
        image_height = std::max(int(image_width/aspect_ratio), 1);

        pixel_samples_scale=1.0/samples_per_pixel;

        center = lookfrom;

        // Camera 
        auto theta = degrees_to_radians(vfov);      // sets vertical fov
        auto h = std::tan(theta/2); 
        auto viewport_height = 2*h*focus_distance;
        auto viewport_width = viewport_height*((double)image_width/image_height);

        // calculate the u,v,w basis vectors for the camera coordinate frame 
        w = unit_vector(lookfrom-lookat); 
        u = unit_vector(cross(vup,w)); 
        v = cross(w,u); 

        // calculate vectors across the horizontal and vertical viewport edges 
        vec3 viewport_u = viewport_width*u;
        vec3 viewport_v = viewport_height*-v;

        // calculate the horizontal and vertical delta vectors from pixel to pixel 
        pixel_delta_u = viewport_u/image_width;
        pixel_delta_v = viewport_v/image_height;

        // calculate location of upper left pixel 
        auto viewport_upper_left = center - (focus_distance*w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left+0.5*(pixel_delta_u+pixel_delta_v);

        // calculate the camera disk basis vectors 
        auto defocus_radius = focus_distance*std::tan(degrees_to_radians(defocus_angle)); 
        defocus_disk_u = u*defocus_radius; 
        defocus_disk_v = v*defocus_radius;
    }

    ray get_ray(int i,int j) const { 
        // Construct a camera ray originating from defocus disk and directed at randomly sampled points around pixel (i,j) 
        auto offset=sample_square(); 
        auto pixel_sample=pixel00_loc+((i+offset.x())*pixel_delta_u)+((j+offset.y())*pixel_delta_v); 
        auto ray_origin=defocus_angle<=0 ? center : defocus_disk_sample(); 
        auto ray_direction=pixel_sample-ray_origin; 

        return ray(ray_origin,ray_direction);
    }

    vec3 sample_square() const {  
        // Returns the vector to a random point in the range [-0.5,0.5]
        return vec3(random_double()-0.5,random_double()-0.5,0);
    }

    // returns a random point in the defocus disk
    point3 defocus_disk_sample() const { 
        auto p=random_in_unit_disk(); 
        return center+(p[0]*defocus_disk_u)+(p[1]*defocus_disk_v); 
    }

    color ray_color(const ray& r, int depth, const hittable& world) const { 
        // base case 
        if(depth<=0) { 
            return color(0,0,0); 
        }
        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered; 
            color attenuation; 
            if(rec.mat->scatter(r,rec,attenuation,scattered)) {
                return attenuation * ray_color(scattered, depth-1, world); 
            } 
            return color(0,0,0); 
        }
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y()+1.0);
        return (1-a)*color(1.0, 1.0, 1.0)+a*color(0.5, 0.7, 1.0);
    }
};

#endif
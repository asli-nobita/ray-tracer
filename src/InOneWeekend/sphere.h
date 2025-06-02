#ifndef SPHERE_H 
#define SPHERE_H 

#include "hittable.h" 
#include "vec3.h"

class sphere : public hittable {
    public:
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) { }
    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        vec3 oc = center-r.origin();
        vec3 d = r.direction();
        double a = d.length()*d.length();
        double h = dot(d, oc);      // put h=-b/2
        double c = oc.length()*oc.length()-radius*radius;
        auto discriminant = h*h-a*c;
        if (discriminant<0) {
            return false;
        }
        auto sqrtd=std::sqrt(discriminant); 
        auto root=(h-sqrtd)/a; 
        if(root<=ray_tmin || root>=ray_tmax) { 
            root=(h+sqrtd)/a; 
            if(root<=ray_tmin || root>=ray_tmax) return false;
        }
        rec.t=root; 
        rec.p=r.at(rec.t); 
        rec.normal=(rec.p-center)/radius; 

        return true;
    }

    private:
    point3 center;
    double radius;
};

#endif
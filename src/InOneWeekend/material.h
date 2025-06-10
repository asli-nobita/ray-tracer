#ifndef MATERIAL_H 
#define MATERIAL_H 

#include "hittable.h"
#include "ray.h"
#include "color.h"

class material {
    public:
    virtual ~material() = default;
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
    public:
    lambertian(const color& albedo) : albedo(albedo) { }

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal+random_unit_vector();
        // catch degenerate scatter direction (if rec.normal + random unit vector is almost a zero vector) 
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }
    private:
    color albedo;
};

class metal : public material {
    public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(std::min(fuzz, 1.0)) { }

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected)+fuzz*random_unit_vector();
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal)>0);
    }
    private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
    public:
    dielectric(double refractive_index) : refractive_index(refractive_index) { }

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1, 1, 1);
        double ri = rec.front_face ? 1.0/refractive_index : refractive_index;   // doubt: how do we specify new ri if dielectric is placed in a non-air medium? 
        vec3 unit_direction = unit_vector(r_in.direction());
        vec3 refracted = refract(unit_direction, rec.normal, ri);
        scattered = ray(rec.p, refracted); 
        return true; 
    }
    private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refractive_index;
};

#endif
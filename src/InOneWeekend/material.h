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
        attenuation = albedo;       // what fraction of color a material absorbs
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
    dielectric(double refractive_index) : refractive_index(refractive_index) { }    // here, refractive index is actually ri of material / ri of surrounding medium

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1, 1, 1);
        double ri = rec.front_face ? 1.0/refractive_index : refractive_index;   
        vec3 unit_direction = unit_vector(r_in.direction());

        // check if total internal reflection would occur 
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1-cos_theta*cos_theta);

        bool cannot_refract = ri*sin_theta > 1.0; 
        vec3 direction;
        if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        }
        else {
            direction = refract(unit_direction, rec.normal, ri);
        }

        scattered = ray(rec.p, direction);
        return true;
    }
    private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refractive_index; 

    // Calculates reflectance using Schlick's approximation 
    static double reflectance(double cosine, double refractive_index) { 
        auto r0 = (1-refractive_index)/(1+refractive_index); 
        r0 = r0*r0; 
        return r0 + (1-r0)*std::pow((1-cosine),5); 
    }
};

#endif
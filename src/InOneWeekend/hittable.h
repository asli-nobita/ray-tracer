#ifndef HITTABLE_H 
#define HITTABLE_H 

class material;

// details of a hit e ca
class hit_record {
    public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    // sets the hit_record normal vector
    // outward normal is assumed to be of unit length
    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // ray is outside the sphere if dot product is negative 
        front_face = dot(r.direction(), outward_normal)<0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// any object a ray can intersect with
class hittable {
    public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif
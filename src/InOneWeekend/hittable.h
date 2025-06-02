#ifndef HITTABLE_H 
#define HITTABLE_H 

// details of a hit 
class hit_record {
    public:
    point3 p;
    vec3 normal;
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
    virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

#endif
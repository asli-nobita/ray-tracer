#ifndef VEC3_H 
#define VEC3_H 

class vec3 {
    public:
    double e[3];                // coordinate vector
    vec3() : e{ 0,0,0 } { }         // constructor 
    vec3(double e1, double e2, double e3) : e{ e1,e2,e3 } { }

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }
    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }
    vec3& operator/=(double t) {
        return *this *= 1/t;
    } 

    double length() const { 
        return std::sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]); 
    }

    double length_squared() const {
        return (e[0]*e[0]+e[1]*e[1]+e[2]*e[2]);
    }

    // returns true if the vector is close to zero in all dimensions
    bool near_zero() const { 
        auto s=1e-8; 
        return std::fabs(e[0])<s && std::fabs(e[1])<s && std::fabs(e[2])<s;
    }

    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using point3 = vec3;


inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out<<v.e[0]<<' '<<v.e[1]<<' '<<v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0]+v.e[0], u.e[1]+v.e[1], u.e[2]+v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0]-v.e[0], u.e[1]-v.e[1], u.e[2]-v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0]*v.e[0], u.e[1]*v.e[1], u.e[2]*v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t*v;
}

inline vec3 operator/(const vec3& v, double t) {
    return v*(1.0/t);
}

// calculates the scalar dot-product of two vectors
inline double dot(const vec3& u, const vec3& v) {
    return u.e[0]*v.e[0]+u.e[1]*v.e[1]+u.e[2]*v.e[2];
}

// calculates the cross product of two vectors and returns the resulting vector
inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1]*v.e[2]-u.e[2]*v.e[1],
        u.e[2]*v.e[0]-u.e[0]*v.e[2],
        u.e[0]*v.e[1]-u.e[1]*v.e[0]);
}

// normalizes the vector to unit length
inline vec3 unit_vector(const vec3& v) {
    return v/v.length();
}

// returns a random unit vector that lies inside the sphere
inline vec3 random_unit_vector() {
    while (true) {
        auto p = vec3::random(-1, 1);
        auto lensq=p.length_squared(); 
        // exclude very small vectors, as well as those outside the sphere
        if(1e-160<lensq && lensq<=1) { 
            return p/sqrt(lensq); 
        }
    }
} 

inline vec3 random_in_unit_disk() { 
    while(true) { 
        auto p=vec3(random_double(-1,1),random_double(-1,1),0); 
        if(p.length_squared()<1) return p; 
    }
}

// ensures the unit vector lies on the correct side of the sphere 
inline vec3 random_on_hemisphere(const vec3& normal) { 
    vec3 on_unit_sphere=random_unit_vector(); 
    if(dot(on_unit_sphere,normal)>0) { 
        return on_unit_sphere; 
    } 
    return -on_unit_sphere; 
}

// returns reflected ray from a polished surface
inline vec3 reflect(const vec3& v,const vec3& normal) { 
    vec3 b = normal * dot(v,normal); 
    return v-2*b; 
}

// returns refracted ray from medium i to medium r 
// etai over etar represents the ratio of refractive indices of medium i and medium r
inline vec3 refract(const vec3& uv,const vec3& n,double etai_over_etar) { 
    auto cos_theta = std::fmin(dot(-uv,n),1.0);     // here cos_theta is actually |R|*costheta
    vec3 rout_perp = etai_over_etar*(uv+cos_theta*n); 
    vec3 rout_parallel = -std::sqrt(std::fabs(1-rout_perp.length_squared()))*n; 
    return rout_perp + rout_parallel; 
}

#endif

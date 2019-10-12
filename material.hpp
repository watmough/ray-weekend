#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "hitable.hpp"

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p=2.0f*vec3(drand48(),drand48(),drand48())-vec3(1.0,1.0,1.0);
    } while (p.squared_length()>=1.0);
    return p;
}

class material {
public:
    virtual bool scatter(const ray& r,const hit_record& rec,vec3& attenuation,ray& scattered) const=0;
};

#endif

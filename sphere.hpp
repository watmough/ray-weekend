#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hitable.hpp"

class sphere : public hitable {
public:
    sphere() {}
    sphere(vec3 c,float r,material* m) : center(c), radius(r), mat_ptr(m) {};
    virtual bool hit(const ray& r,float tmin,float tmax,hit_record& rec) const;
    vec3 center;
    float radius;
    material *mat_ptr;
};

bool sphere::hit(const ray& r,float tmin,float tmax,hit_record& rec) const {
    vec3 oc=r.origin()-center;
    float a=dot(r.direction(),r.direction());
    float b=dot(oc,r.direction());
    float c=dot(oc,oc)-radius*radius;
    float discriminant=b*b - a*c;
    if (discriminant>0) {
        float temp=(-b-sqrt(b*b-a*c))/a;
        if (temp<tmax && temp>tmin) {
            rec.t=temp;
            rec.p=r.point_at_parameter(rec.t);
            rec.normal=(rec.p-center)/radius;
            rec.mat_ptr=mat_ptr;
            return true;
        }
        temp=(-b+sqrt(b*b-a*c))/a;
        if (temp<tmax && temp>tmin) {
            rec.t=temp;
            rec.p=r.point_at_parameter(rec.t);
            rec.normal=(rec.p-center)/radius;
            rec.mat_ptr=mat_ptr;
            return true;
        }
    }
    return false;
};

#endif
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "ray.hpp"

vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p=2.f*vec3(drand48(),drand48(),0.f)-vec3(1,1,0);
    } while (dot(p,p)>=1.f);
    return p;
}

class camera {
public:
    camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vfov,float aspect,float aperture,float focus_distance) {
        lens_radius=aperture/2.f;
        float theta=vfov*M_PI/180;
        float half_height=tan(theta/2.0f);
        float half_width=aspect*half_height;
        origin=lookfrom;
        w=unit_vector(lookfrom-lookat);
        u=unit_vector(cross(vup,w));
        v=cross(w,u);
        lower_left_corner=origin-half_width*focus_distance*u-half_height*focus_distance*v-focus_distance*w;
        horizontal=2.f*half_width*focus_distance*u;
        vertical=2.f*half_height*focus_distance*v;
    }
    ray get_ray(float s,float t) { 
        vec3 rd=lens_radius*random_in_unit_disk();
        vec3 offset=u*rd.x()+v*rd.y();
        return ray(origin+offset,lower_left_corner+s*horizontal+t*vertical-origin-offset);
    }

    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u,v,w;
    float lens_radius;
};

#endif

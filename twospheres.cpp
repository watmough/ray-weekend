#include <iostream>
#include "sphere.hpp"
#include "hitablelist.hpp"
#include "float.h"
#include "camera.hpp"

vec3 color(const ray& r,hitable* world) {
    hit_record rec;
    if (world->hit(r,0.0,MAXFLOAT,rec)) {
        return 0.5f*vec3(rec.normal.x()+1,rec.normal.y()+1,rec.normal.z()+1);
    }
    // compute sky background from y
    vec3 unit_direction = unit_vector(r.direction());
    float fade=0.5*(unit_direction.y()+1.0);
    return (1.0f-fade)*vec3(1.0f,1.0f,1.0f)+fade*vec3(0.5f,0.7f,1.0f);
}

int main() {
    int nx=1600;
    int ny=800;
    int ns=20;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *list[2];
    list[0]=new sphere(vec3(0,0,-1),0.5);
    list[1]=new sphere(vec3(0,-100.5,-1),100);
    hitable *world=new hitable_list(list,2);
    camera cam;

    for (int j=ny-1; j>=0; j--) {
        for (int i=0; i<nx; i++) {
            vec3 col(0,0,0);
            for (int s=0;s<ns;s++) {
                float u=float(i+drand48())/float(nx);
                float v=float(j+drand48())/float(ny);
                ray r=cam.get_ray(u,v);
                col+=color(r,world);
            }
            col/=float(ns);
            int ir=int(255.9*col[0]);
            int ig=int(255.9*col[1]);
            int ib=int(255.9*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
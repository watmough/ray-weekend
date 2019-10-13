#include <iostream>
#include "lambertian.hpp"
#include "metal.hpp"
#include "glass.hpp"
#include "sphere.hpp"
#include "hitablelist.hpp"
#include "float.h"
#include "camera.hpp"

vec3 color(const ray& r,hitable* world,int depth) {
    hit_record rec;
    if (world->hit(r,0.001,MAXFLOAT,rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth>0 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)) {
            return attenuation*color(scattered,world,depth-1);
        } else {
            return vec3(0,0,0);
        }
    }
    // compute sky background from y
    vec3 unit_direction = unit_vector(r.direction());
    float fade=0.5*(unit_direction.y()+1.0);
    return (1.0f-fade)*vec3(1.0f,1.0f,1.0f)+fade*vec3(0.5f,0.7f,1.0f);
}

int main() {
    int nx=1600;
    int ny=800;
    int ns=50;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *list[2];
    float R=cos(M_PI/4.0f);
    camera cam(90,float(nx)/float(ny));
    list[0]=new sphere(vec3(-R,0.0f,-1.f),R,new lambertian(vec3(0.f,0.f,1.f)));
    list[1]=new sphere(vec3(R,0.f,-1.f),R,new lambertian(vec3(1.f,0.f,0.f)));
    hitable *world=new hitable_list(list,2);

    for (int j=ny-1; j>=0; j--) {
        for (int i=0; i<nx; i++) {
            vec3 col(0,0,0);
            for (int s=0;s<ns;s++) {
                float u=float(i+drand48())/float(nx);
                float v=float(j+drand48())/float(ny);
                ray r=cam.get_ray(u,v);
                col+=color(r,world,50);
            }
            col/=float(ns);
            col=vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
            int ir=int(255.9*col[0]);
            int ig=int(255.9*col[1]);
            int ib=int(255.9*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
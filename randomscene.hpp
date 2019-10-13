#ifndef RANDOMSCENE_HPP
#define RANDOMSCENE_HPP

#include <iostream>
#include "lambertian.hpp"
#include "metal.hpp"
#include "glass.hpp"
#include "sphere.hpp"
#include "hitablelist.hpp"
#include "float.h"

hitable * random_scene() {
    int n=500;
    hitable **list=new hitable*[n+1];
    list[0]=new sphere(vec3(0,-1000,0),1000,new lambertian(vec3(.5,.5,.5)));
    int i=1;
    for (int a=-11; a<11; a++) {
        for (int b=-11; b<11; b++) {
            float choose_mat=drand48();
            vec3 center(a+0.9f*drand48(),0.2,b+0.9*drand48());
            if ((center-vec3(4,0.2,0)).length()>0.9) {
                if (choose_mat<0.8) { // diffuse 
                    list[i++]=new sphere(center,0.2,new lambertian(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48())));
                } else if (choose_mat<0.95) { // metal
                    list[i++]=new sphere(center,0.2,new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48())),0.5*drand48()));
                } else {
                    list[i++]=new sphere(center,0.2f,new glass(1.5));
                }
            }
        }
    }
    list[i++]=new sphere(vec3(0,1,0),1.0,new glass(1.5));
    list[i++]=new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(0.4,0.2,0.1)));
    list[i++]=new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5),0.0));

    return new hitable_list(list,i);
};

#endif

#include <iostream>
#include "ray.hpp"

vec3 color(const ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    float t=0.5*(unit_direction.y()+1.0);
    return (1.0f-t)*vec3(1.0f,1.0f,1.0f)+t*vec3(0.5f,0.7f,1.0f);
}

int main() {
    int nx=1600;
    int ny=800;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    vec3 lower_left_corner(-2.0,-1.0,-1.0);
    vec3 horizontal(4.0,0.0,0.0);
    vec3 vertical(0.0,2.0,0.0);
    vec3 origin(0.0,0.0,0.0);
    for (int j=ny-1; j>=0; j--) {
        for (int i=0; i<nx; i++) {
            float u=float(i)/float(nx);
            float v=float(j)/float(ny);
            ray r(origin, lower_left_corner+u*horizontal+v*vertical);
            vec3 col=color(r);
            int ir=int(255.9*col[0]);
            int ig=int(255.9*col[1]);
            int ib=int(255.9*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
}
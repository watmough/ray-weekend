#ifndef RAY_OPENCL_H
#define RAY_OPENCL_H

// ray_opencl.h
// include for host side

// definition for drawing space
typedef struct tag_drawspace {
    float lower_left[3];  
    float horizontal[3];
    float vertical[3];
    float origin[3];
} drawspace;

// ray has an origin and direction
typedef struct tag_ray {
    float origin[3];
    float direction[3];
} ray;

#endif
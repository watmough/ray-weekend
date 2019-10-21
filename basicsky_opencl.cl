// basicsky_opencl.cl
// opencl kernel for rendering the background

// definition for drawing space
typedef struct tag_drawspace {
    float3 lower_left;  
    float3 horizontal;
    float3 vertical;
    float3 origin;
} drawspace;

// ray has an origin and direction
typedef struct tag_ray {
    float3 origin;
    float3 direction;
} ray;

// return sky color for a ray
inline float3 color(ray r) {
    float3 unit_direction = normalize(r.direction);
    float t=0.5*(unit_direction.y+1.0);
    return (float3)(1.0f-t)*(float3)(.0f, .8f, .8f)+t*(float3)(0.0f,0.3f,1.0f);
}

// kernel for basicsky_opencl.c
__kernel void basicsky(__global drawspace* draw,
                       __global float3* buffer,
                                const unsigned int nx,
                                const unsigned int ny,
                                const unsigned int count)
{
    // get our 'address'
    unsigned int global_work_item=get_global_id(0);
    if (global_work_item<count) {
        // x,y pixel to calculate
        unsigned int x=global_work_item % nx;
        unsigned int y=global_work_item / nx;

        // u,v coords across draw space 
        float u=(float)x/(float)nx;
        float v=(float)y/(float)ny;

        // create a ray
        ray r = {draw->origin, draw->lower_left+u*draw->horizontal+v*draw->vertical};

        // get ray color
        float3 c=color(r);

        // write color to buffer
        buffer[y*nx+x]=c;
    }
}


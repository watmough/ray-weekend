// basicsky_opencl.cl
//
// opencl kernel for rendering the background

// definition for drawing space
typedef struct drawspace {
    float4 lower_left;
    float4 horizontal;
    float4 vertical;
    float4 origin;
} drawspace;

// ray has an origin and direction
typedef struct ray {
    float4 origin;
    float4 direction;
}

// return sky color for a ray
float4 color(const ray& r) {
    float4 unit_direction = normalize(r.direction);
    float t=0.5*(unit_direction.y+1.0);
    return (1.0f-t)*float4(1.f,1.f,1.f,0.f)+t*float4(0.5f,0.7f,1.0f,0.f);
}

// kernel for basicsky_opencl.c
__kernel void basicsky( _global const drawspace* const draw,
                                const unsigned int nx,
                                const unsigned int ny,
                                const unsigned int count,
                        _global float3* buffer)
{
    // get our 'address'
    int global_work_item=get_global_id(0);
    if (global_work_item<count) {
        // x,y pixel to calculate
        int x=global_work_item % draw->nx;
        int y=global_work_item / draw->ny;

        // u,v coords across draw space 
        float u=(float)x/(float)nx;
        float v=(float)y/(float)ny;

        // create a ray
        ray r(draw->origin, draw->lower_left_corner+u*draw->horizontal+v*draw->vertical);

        // get ray color
        float3 c=color(r);

        // write color to buffer
        buffer[y*nx+x]=c;
    }
}

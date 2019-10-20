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

inline float3 point_at_parameter(ray r,float t) {
    return r.origin+t*r.direction;
}

inline float hit_sphere(float3 center,float radius,ray r) {
    float3 oc=r.origin-center;
    float a=dot(r.direction,r.direction);
    float b=2.f*dot(oc,r.direction);
    float c=dot(oc,oc)-radius*radius;
    float discriminant=b*b - 4*a*c;
    if (discriminant<0.f) {
        return -1.0f;
    } else {
        return (-b-sqrt(discriminant))/(2.f*a);
    }
}

// return sky color for a ray
inline float3 color(ray r) {
    // shade sphere if it was hit
    float t=hit_sphere((float3)(0.f,0.f,-1.f), .5f, r);
    if (t>0.f) {
        float3 N=normalize(point_at_parameter(r,t)-(float3)(0.f,0.f,-1.f));
        return 0.5f*(float3)(N.x+1.f, N.y+1.f, N.z+1.f);
    }
    // otherwise shade sky background from ray y
    float3 unit_direction = normalize(r.direction);
    float fade=0.5*(unit_direction.y+1.0);
    return (float3)(1.0f-fade)*(float3)(.0f, .8f, .8f)+fade*(float3)(0.f,0.3f,1.0f);
    // vec3 unit_direction = unit_vector(r.direction());
    // float fade=0.5*(unit_direction.y()+1.0);
    // return (1.0f-fade)*vec3(1.0f,1.0f,1.0f)+fade*vec3(0.5f,0.7f,1.0f);
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

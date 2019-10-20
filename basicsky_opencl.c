// basicsky_opencl.c
// draw the sky background to a file

#include <stdlib.h>
#include <assert.h>

#include "compile-opencl.c"

// definition for drawing space
typedef struct tag_drawspace {
    float lower_left[4];  
    float horizontal[4];
    float vertical[4];
    float origin[4];
} drawspace;

// ...
int main(int argc, char** argv) {
    // check usage
    if (argc!=2) {
        printf("usage: %s file\n",argv[0]);
        printf("please include the kernel to call.\n");
        exit(-1);
    }

    // image size
    int nx=1600;
    int ny=800;

    // ray trace dimensions
    drawspace host_draw = {{-2.f,-1.f,-1.f,0.f},        // lower left
                           {4.f,0.f,0.f,0.f},           // horizontal
                           {0.f,2.f,0.f,0.f},           // vertical
                           {0.f,0.f,0.f,0.f}};          // origin

    // get a context
    opencl *ocl=NULL;
    int err=get_opencl_context(&ocl);

    // load the kernel source and compile
    void *KernelSource=readkernel(argv[1]);
    cl_program program = compilekernel(ocl,KernelSource);
    cl_kernel kernel = clCreateKernel(program, "basicsky", &err);   // ### name
    if (err==CL_SUCCESS) {
        printf("got a kernel.\n");
    } else {
        printf("Error: clCreateKernel returned %ld\n",(long)err);
        exit(err);
    }

    // buffers on host and device sides
    float * host_output=(float *)malloc(nx*ny*4*sizeof(float));
    cl_mem device_draw   = clCreateBuffer(ocl->context, CL_MEM_READ_WRITE, sizeof(drawspace), NULL,&err);
    cl_mem device_output = clCreateBuffer(ocl->context, CL_MEM_READ_WRITE, nx*ny*4*sizeof(float), NULL, NULL);
    if (!host_output||!device_output||err!=CL_SUCCESS) {
        printf("Error: failed to allocate host or device memory buffer of %ld bytes\n",nx*ny*4*sizeof(float));
        exit(-1);
    } else {
        printf("allocated %ld bytes for device_output and host_output\n",nx*ny*4*sizeof(float));
    }

    // create a command queue
    cl_command_queue commands = clCreateCommandQueue(ocl->context, ocl->device_id, 0, &err);
    if (err==CL_SUCCESS) {
        printf("got a cl_command_queue.\n");
    } else {
        printf("Error: clCreateCommandQueue returned %ld\n",(long)err);
        exit(err);
    }

    // write the data to the gpu
    err = clEnqueueWriteBuffer(commands, device_draw, CL_TRUE, 0, sizeof(drawspace), &host_draw, 0, NULL, NULL);

    // set up args to be passed into the kernel
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_draw);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &device_output);
    clSetKernelArg(kernel, 2, sizeof(unsigned int), &nx);
    clSetKernelArg(kernel, 3, sizeof(unsigned int), &ny);
    size_t work_item_count=nx*ny;
    clSetKernelArg(kernel, 4, sizeof(size_t), &work_item_count);

    // query the work group size for this device_id
    size_t local;
    if ((err=clGetKernelWorkGroupInfo(kernel, ocl->device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL))==CL_SUCCESS) {
        printf("clGetKernelWorkGroupInfo: KERNEL_WORK_GROUP_SIZE: %ld.\n",(long)local);
    } else {
        printf("Error: clGetKernelWorkGroupInfo returned %ld\n",(long)err);
        exit(err);
    }

    // set the total count (global) and work group size (local) and queue the kernel for execution
    size_t requested_work_item_count=work_item_count;//+(work_item_count % local);
    if( (err=clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &requested_work_item_count, /*&local*/NULL, 0, NULL, NULL))==CL_SUCCESS) {
        printf("enqueued the kernel.\n");
    } else {
        printf("Error: clEnqueueNDRangeKernel returned %ld\n",(long)err);
        exit(err);
    }

    // wait for device to complete - sync
    if ((err=clFinish(commands))==CL_SUCCESS) {
        printf("commands in queue completed.\n");
    } else {
        printf("Error: clFinish returned %ld\n",(long)err);
        exit(err);
    }

    // copy output buffer from device to host
    printf("About to read buffer...\n");
    clEnqueueReadBuffer(commands, device_output, /*blocking*/CL_TRUE, 0, work_item_count*4*sizeof(float), 
                        host_output, 0, NULL, NULL);

    // write out data to ppm file
    FILE * out=fopen("/Users/jonathan/Workarea/ray-weekend/image.ppm","w");
    assert(out);
    fprintf(out,"P3\n%ld %ld\n255\n",(long)nx,(long)ny);
    for (int j=ny-1; j>=0; j--) {
        for (int i=0; i<nx; i++) {
            int ir=(int)( 255.9f*host_output[(j*nx+i)*4+0] );
            int ig=(int)( 255.9f*host_output[(j*nx+i)*4+1] );
            int ib=(int)( 255.9f*host_output[(j*nx+i)*4+2] );
            fprintf(out,"%ld %ld %ld\n", (long)ir, (long)ig, (long)ib);
        }
    }
    fclose(out);

    // cleanup
    free(KernelSource);
    clReleaseProgram(program);
    clReleaseCommandQueue(commands);
    dispose_opencl_context(ocl);
    return 0;
}
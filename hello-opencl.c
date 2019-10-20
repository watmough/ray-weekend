// Example OpenCL code 
// Somewhat cleaned-up with error checking

#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#define DATA_SIZE (1024*1024)
const char *KernelSource =
    "__kernel void square(__global float* input, __global float* output, const unsigned int count) { \n" \
    "   int i = get_global_id(0);                                                                    \n" \
    "   if(i < count) { output[i] = input[i] * input[i]; }                                           \n" \
    "}";

int main(void) {
    // try and find a GPU
    int err;
    cl_device_id device_id;
    if ((err=clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL))==CL_SUCCESS) {
        printf("device_id: %ld\n",(long)device_id);
    } else {
        printf("Error: clGetDeviceIDs returned %ld\n",(long)err);
        exit(err);
    }

    // create a context
    cl_context context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    if (err==CL_SUCCESS) {
        printf("got a cl_context.\n");
    } else {
        printf("Error: clCreateContext returned %ld\n",(long)err);
        exit(err);
    }

    // create a command queue
    cl_command_queue commands = clCreateCommandQueue(context, device_id, 0, &err);
    if (err==CL_SUCCESS) {
        printf("got a cl_command_queue.\n");
    } else {
        printf("Error: clCreateCommandQueue returned %ld\n",(long)err);
        exit(err);
    }

    // create our program
    cl_program program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
    if (err==CL_SUCCESS) {
        printf("got a cl_program\n");
    } else {
        printf("Error: clCreateProgramWithSource returned %ld\n",(long)err);
        exit(err);
    }

    // compile it, and print build log if it fails
    if ((err=clBuildProgram(program, 0, NULL, NULL, NULL, NULL))==CL_SUCCESS) {
        printf("compiled successfully.\n");
    } else {
        printf("Error: clBuildProgram returned %ld\n",(long)err);

        // dump the errors
        // https://stackoverflow.com/questions/9464190/error-code-11-what-are-all-possible-reasons-of-getting-error-cl-build-prog
        if (err == CL_BUILD_PROGRAM_FAILURE) {
            // Determine the size of the log
            size_t log_size;
            clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

            // Allocate memory for the log
            char *log = (char *) malloc(log_size);

            // Get the log
            clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

            // Print the log
            printf("Build log:\n%s\n", log);
        }
        exit(err);
    }

    // create a kernel
    cl_kernel kernel = clCreateKernel(program, "square", &err);
    if (err==CL_SUCCESS) {
        printf("got a kernel.\n");
    } else {
        printf("Error: clCreateKernel returned %ld\n",(long)err);
        exit(err);
    }

    // create buffers
    cl_mem input = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * DATA_SIZE, NULL, NULL);
    cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * DATA_SIZE, NULL, NULL);

    // create some data
    float *data = malloc(DATA_SIZE*sizeof(float));
    for (int i = 0; i < DATA_SIZE; i++) { 
        data[i] = i; 
    }

    // write the data to the gpu
    err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * DATA_SIZE, data, 0, NULL, NULL);

    // set up args to be passed into the kernel
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    unsigned int count = DATA_SIZE;
    clSetKernelArg(kernel, 2, sizeof(unsigned int), &count);

    // query the work group size for this device_id
    size_t local;
    if ((err=clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL))==CL_SUCCESS) {
        printf("clGetKernelWorkGroupInfo: KERNEL_WORK_GROUP_SIZE: %ld.\n",(long)local);
    } else {
        printf("Error: clGetKernelWorkGroupInfo returned %ld\n",(long)err);
        exit(err);
    }

    // set the total count (global) and work group size (local) and queue the kernel for execution
    size_t global = count;
    if( (err=clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL))==CL_SUCCESS) {
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

    // read the computed data
    float results[DATA_SIZE];
    clEnqueueReadBuffer(commands, output, CL_TRUE, 0, sizeof(float) * count, results, 0, NULL, NULL);

    // check the data
    unsigned int correct = 0;
    for (int i = 0; i < count; i++) {
        if (results[i]==data[i]*data[i]) { 
            correct++; 
        }
    }
    printf("Computed '%d/%d' correct values!\n", correct, count);
    
    // cleanup
    free(data);
    clReleaseMemObject(input);
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    return 0;
}


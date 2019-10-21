// Example OpenCL code 
// Somewhat cleaned-up with error checking

#include <stdio.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

// read kernel from file
char* readkernel(char *filename) {
    // load the kernel source
    FILE *f=fopen(filename,"r");
    if (!f) {
        printf("Error: unable to open file: %s\n",filename);
        exit(-2);
    }
    int ret=fseek(f,0,SEEK_END);
    if (ret) {
        printf("Error: seek returned %ld\n",(long)ret);
        exit(ret);
    }
    size_t len=ftell(f);
    void *KernelSource=malloc(len);
    if (!f || !len || !KernelSource) {
        printf("Error: bad file: %s or len: %ld or buffer: %p\n",filename,len,KernelSource);
        exit(-2);
    } else {
        printf("file is %ld bytes\n",len);
    }
    fseek(f,0,SEEK_SET);
    size_t count;
    if (len!=(count=fread(KernelSource,1,len,f))) {
        printf("Error: unable to read %ld bytes, got %ld\n",len,count);
        exit(-1);
    }
    fclose(f);
    return KernelSource;
}

typedef struct tag_opencl {
    cl_platform_id      platform_id;
    cl_device_id        device_id;
    cl_context          context;
} opencl;

// initialize an opencl context
int get_opencl_context(opencl** pocl) {
    // allocate an ocl struct
    opencl *ocl=(*pocl=(opencl *)malloc(sizeof(opencl)));
    if (!ocl) {
        printf("Error: failed to allocate an opencl struct\n");
        exit(-1);
    }

    // get platform id
    int err=0;

    // try and find a GPU
    if ((err=clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU/*CPU GPU*/, 1, &ocl->device_id, NULL))==CL_SUCCESS) {
        printf("device_id: %ld\n",(long)ocl->device_id);
    } else {
        printf("Error: clGetDeviceIDs returned %ld\n",(long)err);
        exit(err);
    }

    // create a context
    ocl->context = clCreateContext(0, 1, &ocl->device_id, NULL, NULL, &err);
    if (err==CL_SUCCESS) {
        printf("got a cl_context.\n");
    } else {
        printf("Error: clCreateContext returned %ld\n",(long)err);
        exit(err);
    }
    return err;
};

// dispose the the convenience struct and unref cl_context
void dispose_opencl_context(opencl *ocl) {
    clReleaseContext(ocl->context);
    free(ocl);
}

// compile an opencl kernel and return the resulting handle
cl_program compilekernel(opencl* ocl, char *KernelSource) {
    // create our program
    int err=0;
    cl_program program = clCreateProgramWithSource(ocl->context, 1, (const char **) &KernelSource, NULL, &err);
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
            clGetProgramBuildInfo(program, ocl->device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

            // Allocate memory for the log
            char *log = (char *) malloc(log_size);

            // Get the log
            clGetProgramBuildInfo(program, ocl->device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

            // Print the log
            printf("Build log:\n%s\n", log);
        }
        exit(err);
    }

    // return  the program
    return program;
}

#ifdef MAKE_COCL
// compile an opencl kernel
int main(int argc, char **argv) {
    // check usage
    if (argc==2) {
        printf("compiling file: %s\n",argv[1]);
    } else {
        printf("usage: %s file\n",argv[0]);
        printf("checks if the passed kernel can be compiled.\n");
        exit(-1);
    }

    // get a context
    opencl *ocl=NULL;
    int err=get_opencl_context(&ocl);

    // load the kernel source
    void *KernelSource=readkernel(argv[1]);

    // create our program
    cl_program program = compilekernel(ocl,KernelSource);

    // cleanup
    free(KernelSource);
    clReleaseProgram(program);
    dispose_opencl_context(ocl);
    return 0;
}
#endif

#pragma once

#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <CL/cl_gl.h>
#include <CL/cl_egl.h>
#include <string>
#include <sstream>


namespace ocl {

   const char* error2text(int code) {
        switch (code) {
        // from cl.h
        case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE: return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
        case CL_PROFILING_INFO_NOT_AVAILABLE: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case CL_MEM_COPY_OVERLAP: return "CL_MEM_COPY_OVERLAP";
        case CL_IMAGE_FORMAT_MISMATCH: return "CL_IMAGE_FORMAT_MISMATCH";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
        case CL_MAP_FAILURE: return "CL_MAP_FAILURE";
#ifdef CL_VERSION_1_1
        case CL_MISALIGNED_SUB_BUFFER_OFFSET: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
#endif
#ifdef CL_VERSION_1_2
        case CL_COMPILE_PROGRAM_FAILURE: return "CL_COMPILE_PROGRAM_FAILURE";
        case CL_LINKER_NOT_AVAILABLE: return "CL_LINKER_NOT_AVAILABLE";
        case CL_LINK_PROGRAM_FAILURE: return "CL_LINK_PROGRAM_FAILURE";
        case CL_DEVICE_PARTITION_FAILED: return "CL_DEVICE_PARTITION_FAILED";
        case CL_KERNEL_ARG_INFO_NOT_AVAILABLE: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
#endif
        case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE_TYPE: return "CL_INVALID_DEVICE_TYPE";
        case CL_INVALID_PLATFORM: return "CL_INVALID_PLATFORM";
        case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
        case CL_INVALID_CONTEXT: return "CL_INVALID_CONTEXT";
        case CL_INVALID_QUEUE_PROPERTIES: return "CL_INVALID_QUEUE_PROPERTIES";
        case CL_INVALID_COMMAND_QUEUE: return "CL_INVALID_COMMAND_QUEUE";
        case CL_INVALID_HOST_PTR: return "CL_INVALID_HOST_PTR";
        case CL_INVALID_MEM_OBJECT: return "CL_INVALID_MEM_OBJECT";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case CL_INVALID_IMAGE_SIZE: return "CL_INVALID_IMAGE_SIZE";
        case CL_INVALID_SAMPLER: return "CL_INVALID_SAMPLER";
        case CL_INVALID_BINARY: return "CL_INVALID_BINARY";
        case CL_INVALID_BUILD_OPTIONS: return "CL_INVALID_BUILD_OPTIONS";
        case CL_INVALID_PROGRAM: return "CL_INVALID_PROGRAM";
        case CL_INVALID_PROGRAM_EXECUTABLE: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_DEFINITION: return "CL_INVALID_KERNEL_DEFINITION";
        case CL_INVALID_KERNEL: return "CL_INVALID_KERNEL";
        case CL_INVALID_ARG_INDEX: return "CL_INVALID_ARG_INDEX";
        case CL_INVALID_ARG_VALUE: return "CL_INVALID_ARG_VALUE";
        case CL_INVALID_ARG_SIZE: return "CL_INVALID_ARG_SIZE";
        case CL_INVALID_KERNEL_ARGS: return "CL_INVALID_KERNEL_ARGS";
        case CL_INVALID_WORK_DIMENSION: return "CL_INVALID_WORK_DIMENSION";
        case CL_INVALID_WORK_GROUP_SIZE: return "CL_INVALID_WORK_GROUP_SIZE";
        case CL_INVALID_WORK_ITEM_SIZE: return "CL_INVALID_WORK_ITEM_SIZE";
        case CL_INVALID_GLOBAL_OFFSET: return "CL_INVALID_GLOBAL_OFFSET";
        case CL_INVALID_EVENT_WAIT_LIST: return "CL_INVALID_EVENT_WAIT_LIST";
        case CL_INVALID_EVENT: return "CL_INVALID_EVENT";
        case CL_INVALID_OPERATION: return "CL_INVALID_OPERATION";
        case CL_INVALID_GL_OBJECT: return "CL_INVALID_GL_OBJECT";
        case CL_INVALID_BUFFER_SIZE: return "CL_INVALID_BUFFER_SIZE";
        case CL_INVALID_MIP_LEVEL: return "CL_INVALID_MIP_LEVEL";
        case CL_INVALID_GLOBAL_WORK_SIZE: return "CL_INVALID_GLOBAL_WORK_SIZE";
#ifdef CL_VERSION_1_1
        case CL_INVALID_PROPERTY: return "CL_INVALID_PROPERTY";
#endif
#ifdef CL_VERSION_1_2
        case CL_INVALID_IMAGE_DESCRIPTOR: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case CL_INVALID_COMPILER_OPTIONS: return "CL_INVALID_COMPILER_OPTIONS";
        case CL_INVALID_LINKER_OPTIONS: return "CL_INVALID_LINKER_OPTIONS";
        case CL_INVALID_DEVICE_PARTITION_COUNT: return "CL_INVALID_DEVICE_PARTITION_COUNT";
#endif 
#ifdef CL_VERSION_2_0
        case CL_INVALID_PIPE_SIZE: return "CL_INVALID_PIPE_SIZE";
        case CL_INVALID_DEVICE_QUEUE: return "CL_INVALID_DEVICE_QUEUE";
#endif
#ifdef CL_VERSION_2_2
        case CL_INVALID_SPEC_ID: return "CL_INVALID_SPEC_ID";
        case CL_MAX_SIZE_RESTRICTION_EXCEEDED: return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
#endif

        // from cl_egl.h
        case CL_INVALID_EGL_OBJECT_KHR: return "CL_INVALID_EGL_OBJECT_KHR";
        case CL_EGL_RESOURCE_NOT_ACQUIRED_KHR: return "CL_EGL_RESOURCE_NOT_ACQUIRED_KHR";
        
        // from cl_ext.h
        case CL_INVALID_COMMAND_BUFFER_KHR: return "CL_INVALID_COMMAND_BUFFER_KHR";
        case CL_INVALID_SYNC_POINT_WAIT_LIST_KHR: return "CL_INVALID_SYNC_POINT_WAIT_LIST_KHR";
        case CL_INCOMPATIBLE_COMMAND_QUEUE_KHR: return "CL_INCOMPATIBLE_COMMAND_QUEUE_KHR";
        case CL_PLATFORM_NOT_FOUND_KHR: return "CL_PLATFORM_NOT_FOUND_KHR";
        case CL_CONTEXT_TERMINATED_KHR: return "CL_CONTEXT_TERMINATED_KHR"; 
        case CL_DEVICE_PARTITION_FAILED_EXT: return "CL_DEVICE_PARTITION_FAILED_EXT";
        case CL_INVALID_PARTITION_COUNT_EXT: return "CL_INVALID_PARTITION_COUNT_EXT";
        case CL_INVALID_PARTITION_NAME_EXT: return "CL_INVALID_PARTITION_NAME_EXT";

        case CL_GRALLOC_RESOURCE_NOT_ACQUIRED_IMG: return "CL_GRALLOC_RESOURCE_NOT_ACQUIRED_IMG";
        case CL_INVALID_GRALLOC_OBJECT_IMG: return "CL_INVALID_GRALLOC_OBJECT_IMG";
        case CL_INVALID_SEMAPHORE_KHR: return "CL_INVALID_SEMAPHORE_KHR";

        case CL_COMMAND_TERMINATED_ITSELF_WITH_FAILURE_ARM: return "CL_COMMAND_TERMINATED_ITSELF_WITH_FAILURE_ARM";
        case CL_INVALID_ACCELERATOR_INTEL: return "CL_INVALID_ACCELERATOR_INTEL";
        case CL_INVALID_ACCELERATOR_TYPE_INTEL: return "CL_INVALID_ACCELERATOR_TYPE_INTEL";
        case CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL: return "CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL";
        case CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL: return "CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL";
        
        // from cl_gl.h
        case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR"; 
        }

        return "<unknown error code>";
    }

    struct clexception : std::exception {
        std::stringstream text;
        std::string copy;
        cl_int ret;

        clexception(cl_int ret) : ret(ret) {
        }

        clexception(const char* f, cl_int ret) : ret(ret) {
            text << "Function " << f << " code " << error2text(ret) << " (" << ret << ")";
            copy = text.str();
        }

        const char* what() const noexcept override {
            return copy.c_str();
        }
    }; 

    template<typename T>
    clexception& operator<<(clexception& e, T t) {
        e.text << t;
        e.copy = e.text.str();
        return e;
    }

    template<>
    clexception& operator<<(clexception& e, const char* t) {
        e.text << t;
        e.copy = e.text.str();
        return e;
    }
}




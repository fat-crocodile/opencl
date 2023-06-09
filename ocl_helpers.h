#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <CL/cl.h>        

#include "ocl_error.h"

namespace ocl {
    
    struct context {
        cl_context ctx;
        cl_device_id did;

        context(cl_device_id id) {
            cl_int ret;
            ctx = clCreateContext(NULL, 1, &id, NULL, NULL, &ret);
            if (ret != CL_SUCCESS)
                throw clexception("clCreateContext", ret);
            did = id;
        }

        ~context() {
            clReleaseContext(ctx);
        }

        cl_mem create_buffer(cl_mem_flags flag, size_t sz) {
            cl_int ret = 0;
            cl_mem res = clCreateBuffer(ctx, flag, sz, NULL, &ret);
            if (ret != CL_SUCCESS)
                throw clexception("clCreateBuffer", ret);
            return res;
        }

        cl_command_queue create_queue() {
            cl_int ret = 0;
            cl_command_queue queue = clCreateCommandQueue(ctx, did, 0, &ret);
            if (ret != CL_SUCCESS)
                throw clexception("clCreateCommandQueue", ret);
            return queue;
        }

        cl_program create_program(const char* code, const char* options=NULL) {
            cl_int ret = 0;

            const char* code_arr[] = {code};
            size_t code_sizes[] = {std::strlen(code)};

            cl_program program = clCreateProgramWithSource(ctx, 1, code_arr, code_sizes, &ret); 
            if (ret != CL_SUCCESS)
                throw clexception("clCreateProgramWithSource", ret);
        
            ret = clBuildProgram(program, 1, &did, options, NULL, NULL); 
            if (ret != CL_SUCCESS) {
                clexception e(ret);
                e << "clBuildProgram returns " << ocl::error2text(ret) << " (" << ret << ")\n";
                e << "clGetProgramBuildInfo:\n"; 
                e << "status:  " << getProgramBuildInfo(program, did, CL_PROGRAM_BUILD_STATUS) << "\n";
                e << "options: " << getProgramBuildInfo(program, did, CL_PROGRAM_BUILD_OPTIONS) << "\n";
                e << "log:\n" << getProgramBuildInfo(program, did, CL_PROGRAM_BUILD_LOG) << "\n";
                throw e;
            }

            return program;
        }

        static std::string getProgramBuildInfo(cl_program program, cl_device_id did, cl_program_build_info param) {
            if (param == CL_PROGRAM_BUILD_STATUS) {
                // status to string
                cl_build_status st;
                cl_int ret = clGetProgramBuildInfo(program, did, param, sizeof(st), &st, 0);
                if (ret != CL_SUCCESS)
                    throw clexception("clGetProgramBuildInfo", ret);

                switch (st) {
                case CL_BUILD_NONE: return "CL_BUILD_NONE";
                case CL_BUILD_ERROR: return "CL_BUILD_ERROR";
                case CL_BUILD_SUCCESS: return "CL_BUILD_SUCCESS";
                case CL_BUILD_IN_PROGRESS: return "CL_BUILD_ON_PROGRESS";
                }
                return "<unknown>";
            }
            
            size_t sz = 0;
            cl_int ret = clGetProgramBuildInfo(program, did, param, 0, 0, &sz);
            if (ret != CL_SUCCESS)
                throw clexception("clGetProgramBuildInfo", ret);

            std::string res;
            res.resize(sz);
            ret = clGetProgramBuildInfo(program, did, param, sz, &res[0], &sz);        
            if (ret != CL_SUCCESS)
                throw clexception("clGetProgramBuildInfo", ret);
            while (res.size() && res.back() == 0)
                res.pop_back();
            return res;
        }
    };


    struct command_queue {
        cl_command_queue q;

        command_queue(cl_command_queue q) : q(q) {}
        ~command_queue() {
            clReleaseCommandQueue(q);
        }

        void finish() {
            clFlush(q);
            clFinish(q);
        }

        template<typename T>
        size_t write_buffer(cl_mem m, const std::vector<T>& v) {
            return write_buffer(m, 0, &v[0], v.size() * sizeof(T), false);
        }

        template<typename T>
        size_t write_buffer_async(cl_mem m, size_t offset, const std::vector<T>& v) {
            return write_buffer(m, offset, &v[0], v.size() * sizeof(T), true);
        }

        size_t write_buffer(cl_mem m, size_t offset, const void* data, size_t sz, bool sync) {
            cl_int ret = 0;
            ret = clEnqueueWriteBuffer(q, m, sync ? CL_TRUE : CL_FALSE, offset, sz, data, 0, NULL, NULL);
            if (ret != CL_SUCCESS)
                throw clexception("clEnqueueWriteBuffer", ret);
            return sz;
        }


        void run1d(cl_kernel kernel, size_t range, size_t ws = 0) {
            run(kernel, 1, &range, (ws != 0) ? &ws : NULL);
        }

        void run2d(cl_kernel kernel, size_t range1, size_t range2, size_t ws1 = 0, size_t ws2 = 0) {
            size_t ranges[] = {range1, range2};
            size_t wss[] = {ws1, ws2};
            run(kernel, 2, ranges, (ws1 != 0) ? wss : NULL);
        }

        void run(cl_kernel kernel, size_t ndims, size_t* range, size_t* ws) {
            cl_int ret = clEnqueueNDRangeKernel(q, kernel, ndims, NULL, range, ws, 0, NULL, NULL);
            if (ret != CL_SUCCESS)
                throw clexception("clEnqueueNDRangeKernel", ret);            
        }


        template<typename T>
        void read_buffer(cl_mem m, std::vector<T>* p) {
            read_buffer(m, 0, &((*p)[0]), p->size() * sizeof(T));
        } 

        template<typename T>
        size_t read_buffer_async(cl_mem m, size_t offset, std::vector<T>* p) {
            return read_buffer(m, offset, &((*p)[0]), p->size() * sizeof(T), false);
        } 

        size_t read_buffer(cl_mem m, size_t offset, void* p, size_t sz, bool sync=true) {
            cl_int ret = clEnqueueReadBuffer(q, m, sync ? CL_TRUE : CL_FALSE, offset, sz, p, 0, NULL, NULL);
            if (ret != CL_SUCCESS)
                throw clexception("clEnqueueReadBuffer", ret);
            return sz;
        } 
    };


    struct kernel {
        cl_kernel k;

        kernel(cl_kernel k) : k(k) {}
        ~kernel() {
            clReleaseKernel(k);
        }

        void setArg(int n, int sz, const void* p) {
            cl_int ret = clSetKernelArg(k, n, sz, p);
            if (ret != CL_SUCCESS)
                throw clexception("clSetKernelArg", ret);
        }
    };


    struct program {
        cl_program p;
        program(cl_program p) : p(p) {}
        ~program() {
            clReleaseProgram(p);
        }

        cl_kernel create_kernel(const char* name) {
            cl_int ret = 0;
            cl_kernel k = clCreateKernel(p, name, &ret);
            if (ret != CL_SUCCESS)
                throw clexception("clCreateKernel", ret);
            return k;
        }
    };


    struct mem_buffer {
        cl_mem m;
        mem_buffer(cl_mem m) : m(m) {}
        ~mem_buffer() {
            clReleaseMemObject(m);
        }
    };
}



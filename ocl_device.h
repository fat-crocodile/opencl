#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <CL/cl.h>        


namespace ocl {

using std::string;
using std::vector;
using std::strlen;
using std::ostream;
using std::sort;


struct device_description {
    cl_device_id id;
    string name;
    cl_uint units;
    cl_device_type type;
    cl_device_fp_config dfp;
    size_t max_work_group;
    cl_device_local_mem_type local_memory_type;
    cl_ulong local_memory_size;
    cl_bool integrated;
};

bool operator<(const device_description& x, const device_description& y) {
    if ((x.type & CL_DEVICE_TYPE_CPU) && (y.type & CL_DEVICE_TYPE_CPU)) {
        return x.units < y.units;
    }
    if ((x.type & CL_DEVICE_TYPE_CPU) && (y.type & CL_DEVICE_TYPE_GPU)) {
        return true;
    }
    if ((x.type & CL_DEVICE_TYPE_GPU) && (y.type & CL_DEVICE_TYPE_CPU)) {
        return false;
    }
    if ((x.type & CL_DEVICE_TYPE_GPU) && (y.type & CL_DEVICE_TYPE_GPU)) {
        if (x.integrated == CL_TRUE && y.integrated != CL_TRUE)
            return true;
        if (x.integrated != CL_TRUE && y.integrated == CL_TRUE)
            return false;
        return x.units < y.units;
    }

    return false;
}


ostream& operator<<(ostream& str, const device_description& dd) {
    str << dd.name << ", units=" << dd.units << ", double=" << dd.dfp << ", type=" << dd.type << " ( ";
    if (dd.type & CL_DEVICE_TYPE_GPU)
        str << "gpu ";
    if (dd.type & CL_DEVICE_TYPE_CPU)
        str << "cpu ";
    if (dd.type & CL_DEVICE_TYPE_ACCELERATOR)
        str << "accelerator ";
    if (dd.type & CL_DEVICE_TYPE_DEFAULT)
        str << "default ";
    if (dd.type & CL_DEVICE_TYPE_CUSTOM)
        str << "custom ";
    str << ") ";
    str << ((dd.integrated == CL_TRUE) ?  "integrated " : "discrete ");

    str << "max_work_group=" << dd.max_work_group << ", local_memory_type=" << dd.local_memory_type << " (";
    if (dd.local_memory_type == CL_LOCAL)
        str << "local";
    else if (dd.local_memory_type == CL_GLOBAL)
        str << "global";
    else
        str << "unknown";

    str << "), local_memory_size=" << dd.local_memory_size; 

    return str;
}


template<typename T>
T get_device_data(cl_device_id id, cl_device_info param) {
    T v = 0;
    auto res = clGetDeviceInfo(id, param, sizeof(v), &v, 0);
    if (res != CL_SUCCESS)
        throw res;
    return v;
}

template<>
string get_device_data<string>(cl_device_id id, cl_device_info param) {
    size_t sz = 0;
    auto res = clGetDeviceInfo(id, param, 0, 0, &sz);
    if (res != CL_SUCCESS)
        throw res;

    string v;
    v.resize(sz);
    res = clGetDeviceInfo(id, param, sz, &v[0], 0);

    if (res != CL_SUCCESS)
        throw res;

    return v;
}

auto get_devices(cl_platform_id pid) {
    vector<device_description> res;

    cl_uint count = 0;
    auto ret = clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, 0, NULL, &count);
    if (ret != CL_SUCCESS) {
        return res;
    }

    vector<cl_device_id> devices(count);
    clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, count, &devices[0], NULL);

    for (const auto& did : devices) {
        device_description dd;
        dd.id = did;
        dd.name = get_device_data<string>(did, CL_DEVICE_NAME);
        dd.units = get_device_data<cl_uint>(did, CL_DEVICE_MAX_COMPUTE_UNITS);
        dd.type = get_device_data<cl_device_type>(did, CL_DEVICE_TYPE);
        dd.dfp = get_device_data<cl_device_fp_config>(did, CL_DEVICE_DOUBLE_FP_CONFIG);
        dd.max_work_group = get_device_data<size_t>(did, CL_DEVICE_MAX_WORK_GROUP_SIZE);
        dd.local_memory_type = get_device_data<cl_device_local_mem_type>(did, CL_DEVICE_LOCAL_MEM_TYPE);
        dd.local_memory_size = get_device_data<cl_ulong>(did, CL_DEVICE_LOCAL_MEM_SIZE);
        dd.integrated = get_device_data<cl_bool>(did, CL_DEVICE_HOST_UNIFIED_MEMORY);
        res.push_back(dd);
    }

    return res; 
}


auto get_devices() {
    cl_uint count;     
    cl_int ret = clGetPlatformIDs(0, NULL, &count);
    if (ret != CL_SUCCESS)
        throw std::runtime_error("can not initialize OpenCL");

    vector<cl_platform_id> platforms(count);
    clGetPlatformIDs(count, &platforms[0], NULL);

    vector<device_description> res;

    for (auto& pid : platforms) {
        auto r = get_devices(pid);
        res.insert(res.end(), r.begin(), r.end());
    }

    sort(res.begin(), res.end(), [](auto x, auto y) {return y < x;});
    return res;
}

}

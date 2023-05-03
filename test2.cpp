#define CL_TARGET_OPENCL_VERSION 120
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <tuple>
#include <CL/cl.h> 
#include "ocl_helpers.h"       
#include "ocl_device.h"

using namespace std;
using namespace ocl;

const char* kernel_two_dims_t = R"(
__kernel void mx_mul(int rows, int cols, int to_sum, __global const float *A, __global const float *B, __global float *C)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    float x = 0;

    for (int k = 0; k < to_sum; ++k)
        x += A[i * to_sum + k] * B[j * to_sum + k];

    C[i * cols + j] = x;
}
)";

using cl_item = cl_float;
using Matrix = vector<vector<float>>;


struct timer {
    timespec s = {0, 0};
    timespec e = {0, 0};

    timer() {
        start();    
    }

    void start() {
        timespec_get(&s, TIME_UTC);    
    }

    void stop() {
        timespec_get(&e, TIME_UTC);    
    }

    double get_ms() {
        if (e.tv_sec == 0 && e.tv_nsec == 0)
            stop();
        return ((e.tv_sec - s.tv_sec) * 1000) + ((e.tv_nsec - s.tv_nsec) / 1000000.0);
    }
};


Matrix transpose_multiplication(const Matrix& m1, const Matrix& m2) {
    Matrix res(m1.size());

    using item_type = Matrix::value_type::value_type;

    const auto rows = m1.size();
    const auto cols = m2.size();
    const auto to_sum = m1[0].size();

    for (size_t i = 0; i < rows; ++i) {
        res[i].resize(cols);
        auto& r = m1[i];
        for (size_t j = 0; j < cols; ++j) {
            auto& c = m2[j];
            item_type x = 0;
            for (size_t k = 0; k < to_sum; ++k)
                x += r[k] * c[k];
            res[i][j] = x;
        }
    }

    return res;
}


auto ocl_simple_multiplication(cl_device_id did, const Matrix& m1, const Matrix& m2t, const char* kernel_code) {
    Matrix res(m1.size());
    timer t;

    int rows = m1.size();
    int cols = m2t.size();
    int to_sum = m1[0].size();

    if (rows % 8 != 0 || cols % 8 != 0) {
        throw runtime_error("this simple code can't multiply matrixes which sizes are not divisible by 8");
    }

    for (size_t i = 0; i < rows; ++i) {
        res[i].resize(cols);
    }
 
    context ctx(did);

    mem_buffer a_mem_obj = ctx.create_buffer(CL_MEM_READ_ONLY, m1.size() * m1[0].size() * sizeof(cl_item));
    mem_buffer b_mem_obj = ctx.create_buffer(CL_MEM_READ_ONLY, m2t.size() * m2t[0].size() * sizeof(cl_item));
    mem_buffer c_mem_obj = ctx.create_buffer(CL_MEM_WRITE_ONLY, res.size() * res[0].size() * sizeof(cl_item));

    command_queue queue = ctx.create_queue();

    try {
        program p = ctx.create_program(kernel_code);
        kernel k = p.create_kernel("mx_mul");

        k.setArg(0, sizeof(int), &rows);
        k.setArg(1, sizeof(int), &cols);
        k.setArg(2, sizeof(int), &to_sum);
        k.setArg(3, sizeof(cl_mem), &a_mem_obj.m);
        k.setArg(4, sizeof(cl_mem), &b_mem_obj.m);
        k.setArg(5, sizeof(cl_mem), &c_mem_obj.m);

        size_t offset = 0;
        for (const auto& v : m1) 
            offset += queue.write_buffer_async(a_mem_obj.m, offset, v);

        offset = 0;
        for (const auto& v : m2t) 
            offset += queue.write_buffer_async(b_mem_obj.m, offset, v);

        queue.finish();
        t.start();
        queue.run(k.k, rows, cols, 8, 8);
        queue.finish();
        t.stop();

        offset = 0;
        for (auto& v : res) 
            offset += queue.read_buffer_async(c_mem_obj.m, offset, &v);

        queue.finish();
    }
    catch (clexception& e) {
        cout << "exception! " << e.f << " " << e.ret << endl;
        throw;
    }

	return make_tuple(res, t.get_ms());
}

//
// Helper matrix routines
//

auto random_matrix(int rows, int cols) {
    Matrix m(rows);

    for (auto& r : m) {
        r.resize(cols);
        for (auto& x : r)
            x = ((rand() % 1001) / 1000.) * 10 - 5;
    }

    return m;
}

auto transpose(const Matrix& m) {
    const int rows = m.size();
    const int cols = m[0].size();

    Matrix t(cols);

    for (size_t i = 0; i < cols; ++i) {
        t[i].resize(rows);
        for (size_t j = 0; j < rows; ++j)
            t[i][j] = m[j][i];
    }

    return t;
}

auto maxdiff(const Matrix& m1, const Matrix& m2) {
    if (m1.size() != m2.size() || m1[0].size() != m2[0].size())
        throw runtime_error("maxdiff: different matrix size");

    double x = 0;
    for (size_t i = 0; i < m1.size(); ++i) {
        for (size_t j = 0; j < m1[0].size(); ++j) {
            double d = fabs(m1[i][j] - m2[i][j]);
            if (x < d) x = d;
        }
    }
    return x;
}


int main(int argc, char* argv[])
{
    auto devices = get_devices();

    const int a = (argc > 1) ? atoi(argv[1]) : 1024;
    const int cpu_max = 2048;

    auto m1 = random_matrix(a, a);
    auto m2 = random_matrix(a, a);
    auto m2t = transpose(m2);

    Matrix res;

    {
        timer t;
        auto [m, it] = ocl_simple_multiplication(devices[0].id, m1, m2t, kernel_two_dims_t);
        auto tms = t.get_ms();
        res = std::move(m);
        cout << "OCL: " << it << "ms kernel time; " << tms << " ms whole time\n";        
    }


    if (a <= cpu_max) {
        Matrix res_ref;
        timer t;
        res_ref = transpose_multiplication(m1, m2t);
        cout << "CPU: " << t.get_ms() << "ms\n";

        if (res_ref == res)
            cout << "res_ref == res_ocl" << endl;
        else
            cout << "res_ref != res_ocl, max diff = " << maxdiff(res_ref, res) << endl;
    }
}


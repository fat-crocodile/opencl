#include <iostream>
#include "ocl_device.h"

int main() {
    for (auto d: ocl::get_devices()) {
        std::cout << d << std::endl;
    }
}


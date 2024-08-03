#include <iostream>

#include "win_include.h"
#include "baltic_except.h"
#include "debug_layer.h"

using namespace Baltic;

int main()
{
    int ret = 0;

    try {
        if (FAILED(RoInitialize(RO_INIT_SINGLETHREADED))) {
            throw BalticException("RoInitialize");
        }

        DebugLayer::Init();


    } catch (const BalticException& e) {
        std::cout << e.what() << '\n';
        ret = 1;
    }

    RoUninitialize();

    return ret;
}
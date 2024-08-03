#include <iostream>

#include "auxiliary/win_include.h"
#include "baltic_except.h"
#include "debug/dx_debug_layer.h"
#include "d3d/dx_context.h"

using namespace Baltic;

void aux()
{
    DXContext::Get().GetDevice();
}

int main()
{
    int ret = 0;

    try {
        if (FAILED(RoInitialize(RO_INIT_SINGLETHREADED))) {
            throw BalticException("RoInitialize");
        }

        DXDebugLayer::Init();

        aux();

    } catch (const BalticException& e) {
        std::cout << e.what() << '\n';
        ret = 1;
    }

    RoUninitialize();

    return ret;
}
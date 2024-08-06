#include <iostream>

#include "auxiliary/win_include.h"
#include "auxiliary/baltic_except.h"
#include "auxiliary/dx_window.h"
#include "auxiliary/constants.h"
#include "debug/dx_debug_layer.h"
#include "d3d/dx_context.h"

using namespace Baltic;

int main()
{
    int ret = 0;

    try {
        if (FAILED(RoInitialize(RO_INIT_SINGLETHREADED))) {
            throw BalticException("RoInitialize");
        }

        DXDebugLayer::Init();

        DXWindow mainWindow;

        mainWindow.SetFullscreen(TRUE);

        while (!mainWindow.ShouldClose()) {

            mainWindow.Update();

            if (mainWindow.ShouldResize()) {
                DXContext::Get().Flush(FRAME_COUNT);
                mainWindow.ResizeSwapChain();
            }

            DXContext::Get().ResetCmdList();

            const auto& cmdList = DXContext::Get().GetCmdList();

            DXContext::Get().ExecuteCmdList();

            mainWindow.Present();
        }

        DXContext::Get().Flush(FRAME_COUNT);

    } catch (const BalticException& e) {
        std::cout << e.what() << '\n';
        ret = 1;
    }

    RoUninitialize();

    return ret;
}
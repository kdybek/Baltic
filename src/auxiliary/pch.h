#pragma once

#define NOMINMAX

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#ifdef BALTIC_DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif

namespace Baltic
{
    using Microsoft::WRL::ComPtr;

} // namespace Baltic

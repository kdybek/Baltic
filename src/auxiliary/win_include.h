#pragma once

#define NOMINMAX

#include <Windows.h>
#include <wrl/client.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#ifdef BALTIC_DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif // BALTIC_DEBUG

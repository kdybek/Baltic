#pragma once

#define NOMINMAX

#include <Windows.h>
#include <d3d12.h>
#include <directxmath.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifdef UNICODE
#include <corecrt_wstring.h>
#else
#include <cstdio>
#endif

#ifdef BALTIC_DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif

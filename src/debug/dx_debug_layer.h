#pragma once

#include "auxiliary/baltic_except.h"
#include "auxiliary/win_include.h"

namespace Baltic
{
    class DXDebugLayer
    {
    public:
        ~DXDebugLayer()
#ifdef BALTIC_DEBUG
        {
            OutputDebugStringW(L"--- DXGI living object report ---\n");
            m_dxgiDebug->ReportLiveObjects(
                DXGI_DEBUG_ALL,
                DXGI_DEBUG_RLO_FLAGS(
                    DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL
                )
            );
        }

#else
            = default;
#endif

        DXDebugLayer(const DXDebugLayer&) = delete;

        DXDebugLayer& operator=(const DXDebugLayer&) = delete;

        inline static void Init() { static DXDebugLayer s_dxDebugLayer; }

    private:
        DXDebugLayer()
#ifdef BALTIC_DEBUG
        {
            if (D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug))) {
                throw BalticException("D3D12GetDebugInterface");
            }

            m_d3d12Debug->EnableDebugLayer();

            if (FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)))) {
                throw BalticException("DXGIGetDebugInterface1");
            }

            m_dxgiDebug->EnableLeakTrackingForThread();
        }

#else
            = default;
#endif

    private:
#ifdef BALTIC_DEBUG
        Microsoft::WRL::ComPtr<ID3D12Debug3> m_d3d12Debug;
        Microsoft::WRL::ComPtr<IDXGIDebug1> m_dxgiDebug;
#endif
    };

}  // namespace Baltic

#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include "auxiliary/baltic_exception.h"

namespace Baltic
{
    class DXDebugLayer
    {
    public:
        DXDebugLayer()
#ifdef BALTIC_DEBUG
        {
            ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug)));

            m_d3d12Debug->EnableDebugLayer();

            ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)));

            m_dxgiDebug->EnableLeakTrackingForThread();
        }
#else
            = default;
#endif

        ~DXDebugLayer() = default;

        DXDebugLayer(const DXDebugLayer&) = delete;

        DXDebugLayer& operator=(const DXDebugLayer&) = delete;

        void ReportLiveObjects()
        {
#ifdef BALTIC_DEBUG
            OutputDebugStringW(L"--- DXGI living object report ---\n");
            ThrowIfFailed(m_dxgiDebug->ReportLiveObjects(
                DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
            ));
#endif
        }

    private:
#ifdef BALTIC_DEBUG
        ComPtr<ID3D12Debug3> m_d3d12Debug;
        ComPtr<IDXGIDebug1> m_dxgiDebug;
#endif
    };

} // namespace Baltic

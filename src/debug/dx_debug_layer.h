#pragma once

#include "auxiliary/baltic_except.h"
#include "auxiliary/types.h"

namespace Baltic
{
    class DXDebugLayer
    {
    public:
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

        ~DXDebugLayer() = default;

        DXDebugLayer(const DXDebugLayer&) = delete;

        DXDebugLayer& operator=(const DXDebugLayer&) = delete;

        void ReportLiveObjects()
        {
#ifdef BALTIC_DEBUG
            OutputDebugStringW(L"--- DXGI living object report ---\n");
            if (FAILED(m_dxgiDebug->ReportLiveObjects(
                    DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
                ))) {
                throw BalticException("m_dxgiDebug->ReportLiveObjects");
            }
#endif
        }

    private:
#ifdef BALTIC_DEBUG
        D3D12Debug3ComPtr m_d3d12Debug;
        DXGIDebug1ComPtr m_dxgiDebug;
#endif
    };

}  // namespace Baltic

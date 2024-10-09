#pragma once

// clang-format off
#include "auxiliary/pch.h"
// clang-format on

#include "auxiliary/baltic_exception.h"
#include "auxiliary/types.h"

class DXDebugLayer
{
public:
    DXDebugLayer()
#ifdef BALTIC_DEBUG
    {
        DXThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug)));

        m_d3d12Debug->EnableDebugLayer();

        DXThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)));

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
        OutputDebugString(TEXT("--- DXGI living object report ---\n"));
        DXThrowIfFailed(m_dxgiDebug->ReportLiveObjects(
            DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
        ));
#endif
    }

private:
#ifdef BALTIC_DEBUG
    ComPtr<ID3D12Debug6> m_d3d12Debug;
    ComPtr<IDXGIDebug1> m_dxgiDebug;
#endif
};

#pragma once

#include "win_include.h"
#include "baltic_except.h"

namespace Baltic
{
    class DebugLayer
    {
    public:
        ~DebugLayer()
#ifdef BALTIC_DEBUG
        {
            OutputDebugStringW(L"--- DXGI living object report ---\n");
            m_dxgiDebug->ReportLiveObjects(
                    DXGI_DEBUG_ALL,
                    DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
            );
        }

#else
        = default;
#endif // BALTIC_DEBUG

        DebugLayer(const DebugLayer&) = delete;

        DebugLayer& operator=(const DebugLayer&) = delete;

        inline static void Init()
        {
            static DebugLayer instance;
        }

    private:
        DebugLayer()
#ifdef BALTIC_DEBUG
        : m_d3d12Debug(nullptr), m_dxgiDebug(nullptr)
        {
            if (D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug))) {
                throw BalticException("D3D12GetDebugInterface");
            }

            S_OK;

            m_d3d12Debug->EnableDebugLayer();

            if (FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_dxgiDebug)))) {
                throw BalticException("DXGIGetDebugInterface1");
            }

            m_dxgiDebug->EnableLeakTrackingForThread();
        }

#else
        = default;
#endif // BALTIC_DEBUG

#ifdef BALTIC_DEBUG
        Microsoft::WRL::ComPtr <ID3D12Debug3> m_d3d12Debug; // TODO: Update PC
        Microsoft::WRL::ComPtr <IDXGIDebug1> m_dxgiDebug;
#endif // BALTIC_DEBUG

    }; // DebugLayer

} // Baltic

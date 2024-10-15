#pragma once

// clang-format off
#include "auxiliary/pch.hpp"
// clang-format on

class BalticException
{
public:
    virtual const TCHAR* GetMessage() const = 0;
};

class GenericException : public BalticException
{
public:
    GenericException(const TCHAR* message) : m_message(message) {}

    const TCHAR* GetMessage() const override { return m_message; }

private:
    const TCHAR* m_message;
};

class ComException : public BalticException
{
public:
    ComException(HRESULT hr) : result(hr) {}

    const TCHAR* GetMessage() const override
    {
        static TCHAR s_message[64] = {0};
#ifdef UNICODE
        swprintf_s(s_message, L"Failure with HRESULT of %08X", result);
#else
        sprintf_s(s_message, "Failure with HRESULT of %08X", result);
#endif
        return s_message;
    }

private:
    HRESULT result;
};

inline void DXThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr)) {
        throw ComException(hr);
    }
}

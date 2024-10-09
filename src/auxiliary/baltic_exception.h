#pragma once

#include <stdio.h>

#include <exception>

class BalticException : public std::exception
{
public:
    BalticException(const TCHAR* message) : m_messsage(message) {}

    const TCHAR* GetMsg() const { return m_messsage; }

private:
    const TCHAR* m_messsage;
};

class ComException : public std::exception
{
public:
    ComException(HRESULT hr) : result(hr) {}

    const TCHAR* GetMsg() const
    {
        static TCHAR s_str[64] = {0};
#ifdef UNICODE
        swprintf_s(s_str, L"Failure with HRESULT of %08X", result);
#else
        sprintf_s(s_str, "Failure with HRESULT of %08X", result);
#endif
        return s_str;
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

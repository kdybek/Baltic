#pragma once

#include <stdio.h>

#include <exception>

#include "auxiliary/win_include.h"

namespace Baltic
{
    class BalticException : public std::exception
    {
    public:
        BalticException(const char* message) : msg(message) {}

        const char* what() const override { return msg; }

    private:
        const char* msg;
    };

    class ComException : public std::exception
    {
    public:
        ComException(HRESULT hr) : result(hr) {}

        const char* what() const override
        {
            static char s_str[64] = {0};
            sprintf_s(s_str, "Failure with HRESULT of %08X", result);
            return s_str;
        }

    private:
        HRESULT result;
    };

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr)) {
            throw ComException(hr);
        }
    }

} // namespace Baltic

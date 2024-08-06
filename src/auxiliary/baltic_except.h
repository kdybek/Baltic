#pragma once

#include <exception>
#include <string>

namespace Baltic
{
    class BalticException : public std::exception
    {
    public:
        explicit BalticException(std::string message) : m_errorMessage(std::move(message))
        {
        }

        ~BalticException() noexcept override = default;

        [[nodiscard]] const char* what() const noexcept override
        {
            return m_errorMessage.c_str();
        }

    private:
        std::string m_errorMessage;
    };

} // Baltic
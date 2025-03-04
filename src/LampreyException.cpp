#include "LampreyException.h"
#include <sstream>

LampreyException::LampreyException(int line, const char* file) noexcept : line(line), file(file) {}

const char* LampreyException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* LampreyException::GetType() const noexcept
{
    return "Lamprey Exception";
}

int LampreyException::GetLine() const noexcept
{
    return line;
}

const std::string& LampreyException::GetFile() const noexcept
{
    return file;
}

std::string LampreyException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << file << std::endl << "[Line] " << line;
    return oss.str();
}
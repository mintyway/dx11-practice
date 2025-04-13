#include "Path.h"

#include "Windows.h"

namespace
{
    std::filesystem::path GetExePath()
    {
        wchar_t path[MAX_PATH]{};
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        return {path};
    }
}

std::filesystem::path Path::GetShaderPath(const wchar_t* fileName)
{
    return GetExePath().parent_path() / L"Shaders" / fileName;
}

std::filesystem::path Path::GetModelPath(const wchar_t* fileName)
{
    return GetExePath().parent_path() / L"Models" / fileName;
}

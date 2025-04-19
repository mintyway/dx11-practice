#pragma once

#include <filesystem>
#include <string>
#include <Windows.h>

namespace Path
{
    namespace Detail
    {
        inline std::filesystem::path GetExePath()
        {
            wchar_t path[MAX_PATH]{};
            GetModuleFileNameW(nullptr, path, MAX_PATH);
            return {path};
        }
    }

    [[nodiscard]]
    inline std::filesystem::path GetShaderPath(const std::wstring& fileName)
    {
        return Detail::GetExePath().parent_path() / L"Shaders" / fileName;
    }

    [[nodiscard]]
    inline std::filesystem::path GetModelPath(const std::wstring& fileName)
    {
        return Detail::GetExePath().parent_path() / L"Models" / fileName;
    }
}

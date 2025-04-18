#pragma once

#include <string>
#include <filesystem>

namespace Path
{
    [[nodiscard]]
    std::filesystem::path GetShaderPath(const std::wstring& fileName);

    [[nodiscard]]
    std::filesystem::path GetModelPath(const std::wstring& fileName);
}

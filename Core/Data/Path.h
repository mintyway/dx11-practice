#pragma once

#include <filesystem>

namespace Path
{
    [[nodiscard]]
    std::filesystem::path GetShaderPath(const wchar_t* fileName);

    [[nodiscard]]
    std::filesystem::path GetModelPath(const wchar_t* fileName);
}

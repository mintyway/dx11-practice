#pragma once

#include "ShaderBase.h"

class BasicShader final : public ShaderBase
{
    DECLARE_SHADER(BasicShader, ShaderBase)

public:
    BasicShader() = default;
    ~BasicShader() override = default;

protected:
    void Init(ID3D11Device* device) override;
};

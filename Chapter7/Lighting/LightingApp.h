#pragma once

#include "Core/SphericalCamera.h"

class LightingApp : public SphericalCamera
{
    DECLARE_ENGINE(LightingApp, BaseEngine)

public:
    ~LightingApp() override = default;
    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    LightingApp() = default;

    void Update(float deltaSeconds) override;
    void Render() override;
};

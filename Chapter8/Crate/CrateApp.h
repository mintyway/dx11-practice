#pragma once
#include "Core/Engine/SphericalCamera.h"

class CrateApp : public SphericalCamera
{
    DECLARE_ENGINE(CrateApp, SphericalCamera)

public:
    CrateApp();
    ~CrateApp() override;

    bool Init(HINSTANCE inInstanceHandle) override;
    void OnResize() override;

protected:
    void Update(float deltaSeconds) override;
    void Render() override;
};

#pragma once

#include "BaseEngine.h"
#include <DirectXMath.h>

using namespace DirectX;

class SphericalCamera : public BaseEngine
{
    DECLARE_ENGINE(SphericalCamera, BaseEngine)

public:
    virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) override;

    inline void SetCameraSphericalCoord(float newRadius, float newTheta, float newPhi)
    {
        cameraRadius = newRadius;
        cameraTheta = newTheta;
        cameraPhi = newPhi;
    }

    inline float GetCameraRadius() const { return cameraRadius; }
    inline float GetCameraTheta() const { return cameraTheta; }
    inline float GetCameraPhi() const { return cameraPhi; }
    inline float GetMinCameraRadius() const { return minCameraRadius; }
    inline float GetMaxCameraRadius() const { return maxCameraRadius; }

protected:
    SphericalCamera();

private:
    float cameraRadius = 200.0f;
    float cameraTheta = -XM_PIDIV4;
    float cameraPhi = XM_PIDIV4;
    float minCameraRadius = 0.0f;

    float maxCameraRadius;

    POINT lastMousePosition{};
};

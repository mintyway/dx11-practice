#pragma once

#include "BaseEngine.h"
#include "Data/SphericalCoord.h"

#include <DirectXMath.h>

using namespace DirectX;

class SphericalCamera : public BaseEngine
{
    DECLARE_ENGINE(SphericalCamera, BaseEngine)

public:
    virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) override;

    inline SphericalCoord GetCameraSphericalCoord() { return cameraSphericalCoord; }
    inline void SetCameraSphericalCoord(float newRadius, float newTheta, float newPhi) { cameraSphericalCoord = SphericalCoord{newRadius, newTheta, newPhi}; }

    inline float GetMinCameraRadius() const { return minCameraRadius; }
    inline float GetMaxCameraRadius() const { return maxCameraRadius; }

    inline void SetZoomSpeed(float zoomSpeedPerPixel) { zoomSpeed = zoomSpeedPerPixel; }

protected:
    SphericalCamera();

private:
    SphericalCoord cameraSphericalCoord = SphericalCoord{100.0f, -XM_PIDIV4, XM_PIDIV4};

    float minCameraRadius = 1.001f;
    float maxCameraRadius;

    float zoomSpeed = 0.1f;

    POINT lastMousePosition{};
};

#pragma once

#include "EngineBase.h"
#include "Core/Data/SphericalCoord.h"

#include <DirectXMath.h>

using namespace DirectX;

class SphericalCamera : public EngineBase
{
    DECLARE_ENGINE(SphericalCamera, EngineBase)

public:
    virtual ~SphericalCamera() = default;

    virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) override;

    SphericalCoord GetCameraSphericalCoord() { return cameraSphericalCoord; }
    void SetCameraSphericalCoord(float newRadius, float newTheta, float newPhi) { cameraSphericalCoord = SphericalCoord{newRadius, newTheta, newPhi}; }

    float GetMinCameraRadius() const { return minCameraRadius; }
    float GetMaxCameraRadius() const { return maxCameraRadius; }

    void SetZoomSpeed(float zoomSpeedPerPixel) { zoomSpeed = zoomSpeedPerPixel; }

protected:
    SphericalCamera();

private:
    SphericalCoord cameraSphericalCoord = SphericalCoord{100.0f, -XM_PIDIV4, XM_PIDIV4};

    float minCameraRadius = 1.001f;
    float maxCameraRadius;

    float zoomSpeed = 0.1f;

    POINT lastMousePosition{};
};

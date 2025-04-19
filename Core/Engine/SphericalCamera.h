#pragma once

#include <DirectXMath.h>

#include "EngineBase.h"
#include "Core/Data/SphericalCoord.h"


class SphericalCamera : public EngineBase
{
    DECLARE_ENGINE(SphericalCamera, EngineBase)

public:
    virtual ~SphericalCamera() = default;

    virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseUp(WPARAM buttonState, int x, int y) override;
    virtual void OnMouseMove(WPARAM buttonState, int x, int y) override;

    SphericalCoord GetCameraSphericalCoord() { return cameraSphericalCoord; }
    void SetCameraSphericalCoord(float newRadius, float newTheta, float newPhi) { cameraSphericalCoord = SphericalCoord{.radius = newRadius, .theta = newTheta, .phi = newPhi}; }

    [[nodiscard]] float GetMinCameraRadius() const { return minCameraRadius; }
    [[nodiscard]] float GetMaxCameraRadius() const { return maxCameraRadius; }

    void SetZoomSpeed(float zoomSpeedPerPixel) { zoomSpeed = zoomSpeedPerPixel; }

protected:
    SphericalCamera();

private:
    SphericalCoord cameraSphericalCoord = SphericalCoord{.radius = 100.0f, .theta = -DirectX::XM_PIDIV4, .phi = DirectX::XM_PIDIV4};

    float minCameraRadius = 1.001f;
    float maxCameraRadius;

    float zoomSpeed = 0.1f;

    POINT lastMousePosition{};
};

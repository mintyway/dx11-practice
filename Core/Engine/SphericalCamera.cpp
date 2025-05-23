#include "SphericalCamera.h"

#include <algorithm>
#include <limits>

using namespace DirectX;

SphericalCamera::SphericalCamera() : maxCameraRadius(std::numeric_limits<float>::max()) {}

void SphericalCamera::OnMouseDown(WPARAM buttonState, int x, int y)
{
    lastMousePosition.x = x;
    lastMousePosition.y = y;
    SetCapture(windowHandle);
}

void SphericalCamera::OnMouseUp(WPARAM buttonState, int x, int y)
{
    ReleaseCapture();
}

void SphericalCamera::OnMouseMove(WPARAM buttonState, int x, int y)
{
    const POINT currentMousePosition{x, y};

    if (buttonState & MK_LBUTTON)
    {
        constexpr float rotationSpeed = XMConvertToRadians(0.25f); // 한 픽셀당 1/4도 회전
        const float dx = rotationSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);
        const float dy = rotationSpeed * static_cast<float>(currentMousePosition.y - lastMousePosition.y);

        cameraSphericalCoord.theta -= dx;
        cameraSphericalCoord.phi = std::clamp(cameraSphericalCoord.phi - dy, XMConvertToRadians(0.1f), XM_PI - XMConvertToRadians(0.1f));
    }
    else if (buttonState & MK_RBUTTON)
    {
        const float dx = zoomSpeed * static_cast<float>(currentMousePosition.x - lastMousePosition.x);

        cameraSphericalCoord.radius = std::clamp(cameraSphericalCoord.radius - (dx), minCameraRadius, maxCameraRadius);
    }

    lastMousePosition = currentMousePosition;
}

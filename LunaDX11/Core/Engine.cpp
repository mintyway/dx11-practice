#include "Engine.h"

#include "Utility.h"

void Engine::Render()
{
    Super::Render();

    immediateContext->ClearRenderTargetView(renderTargetView, Colors::Blue);
    immediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    swapChain->Present(0, 0);
}

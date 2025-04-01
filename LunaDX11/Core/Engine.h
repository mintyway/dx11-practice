#pragma once

#include "BaseEngine.h"

class Engine : public BaseEngine
{
    DECLARE_ENGINE(Engine, BaseEngine)

public:
    virtual void Render() override;
};

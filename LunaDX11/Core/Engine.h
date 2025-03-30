#pragma once

#include "BaseEngine.h"

class Engine : public BaseEngine
{
    using Super = BaseEngine;

public:
    static Engine* Get()
    {
        static Engine instance;
        return &instance;
    }

    virtual void Render() override;

protected:
    Engine() = default;
    virtual ~Engine() = default;
};

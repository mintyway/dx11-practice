#pragma once

#include "BaseEngine.h"

class Engine : public BaseEngine
{
public:
    static Engine* Get()
    {
        static Engine instance;
        return &instance;
    }

protected:
    Engine() = default;
    virtual ~Engine() = default;
};

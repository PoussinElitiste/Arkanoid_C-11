#pragma once
#include "ECS.h"

namespace ECS
{
    class Manager;

    /// TODO: used to replace entity/components update/draw in manager
    class System
    {
    public:
        virtual void initialize(const Manager &_manager) = 0;
        virtual void Draw() {};
    };

    class UpdateSystem : public System
    {
    public:
        void initialize(const Manager& _manager) override {};
        virtual void Update(float ft) {};
    };

    class DrawSystem : public System
    {
    public:
        void initialize(const Manager& _manager) override {};
        virtual void Draw(float ft) {};
    };
}
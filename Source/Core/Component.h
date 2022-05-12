#pragma once

namespace ECS
{   
    class Entity;
    struct Component
    {
        Entity& _entity;

        Component(Entity& entity) : _entity(entity) {}
        virtual ~Component() = default;

        virtual void Init() {}
        /// TODO: migrate logic to dedicated systems
        virtual void Update(float) {}
        virtual void Draw() {}
    };
}
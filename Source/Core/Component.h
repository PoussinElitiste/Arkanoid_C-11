#pragma once

namespace ECS
{   
    class Entity;
    class Component
    {
    protected:
        Entity& _entity;
    
        Component(Entity& entity) : _entity(entity) {}

    public:
        virtual ~Component() = default;

        virtual void Init() {}
        /// TODO: migrate logic to dedicated systems
        virtual void Update(float) {}
        virtual void Draw() {}
    };
}
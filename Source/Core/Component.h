#pragma once

namespace ECS
{   
    class Entity;
    struct Component
    {
        Entity& _entity;

        Component(Entity& entity) : _entity(entity) {}
        virtual ~Component() = default;

        virtual void init() {}
        /// TODO: migrate logic to dedicated systems
        virtual void update(float) {}
        virtual void draw() {}
    };
}
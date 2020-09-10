#pragma once

namespace ECS
{
    class Entity;
}

namespace Event
{
    // dedicated to define who will execute logic on Entity
    // ex: an inputHandler or AI can generate or provide a command
    class Command
    {
    public:
        virtual ~Command() = default;
        /// TODO: must be call by a system
        virtual void execute(ECS::Entity &) = 0;
        virtual void undo() = 0;
    };
}
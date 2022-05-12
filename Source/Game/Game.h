#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "Arkanoid_Global.h"

#include "Manager.h"

using namespace ECS;
namespace ECS {
    class Entity;
    class System;
}

namespace Arkanoid
{
    class Game
    {
        // we define group to accelerate testing
        enum ArkanoidGroup : uint
        {
            GPaddle,
            GBrick,
            GBall
        };

        sf::RenderWindow _window;

        Frametime _lastFt = 0.f;

        // our frame counter
        Frametime _currentSlice = 0.f;

        bool _running = false;

        // handle all entities
        Manager _manager;

        void processCollisionPB(Entity& paddle, Entity& ball);

        void processCollisionBB(Entity& brick, Entity& ball);

    public:
        // factory
        Entity& createBall();
        Entity& createBrick(const sf::Vector2f& position);
        Entity& createPaddle();
        System& createSystem();

        Game();

        void run();
        void inputPhase();

        void updatePhase();

        void drawPhase();
        void render(const sf::Drawable& mDrawable);
    };
}
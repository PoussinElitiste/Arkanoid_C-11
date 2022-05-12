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

        sf::RenderWindow window{ { windowWidth, windowHeight }, "Arkanoid - components" };

        Frametime lastFt{ 0.f };

        // our frame counter
        Frametime currentSlice{ 0.f };

        bool running{ false };

        // handle all entities
        Manager manager;

        void processCollisionPB(Entity& mPaddle, Entity& mBall);

        void processCollisionBB(Entity& mBrick, Entity& mBall);

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
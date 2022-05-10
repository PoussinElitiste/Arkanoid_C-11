#pragma once

#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "CMath.h"
#include "Game.h"
#include "Component.h"
#include "Entity.h"
#include "Observer.h"
#include "System.h"
#include "Game.h"

using namespace ECS;

namespace Arkanoid
{
    CPosition::CPosition(Entity &entity, const sf::Vector2f &position)
        : Component(entity), _position{ position }
    {}

    void CCircle::draw() 
    { game->render(shape); }

    void CRectangle::draw()
    {
        game->render(shape);
    }

    void processCollisionPB(Entity& mPaddle, Entity& mBall)
    {
        auto& cpBall = mBall.getComponent<CPhysics>();
        auto& cpPaddle = mPaddle.getComponent<CPhysics>();

        if (!CMath::isIntersecting(cpPaddle, cpBall)) return;
        cpBall.velocity.y = -ballVelocity;
        if (cpBall.x() < cpPaddle.x()) cpBall.velocity.x = -ballVelocity;
        else cpBall.velocity.x = ballVelocity;

    }

    void processCollisionBB(Entity& mBrick, Entity& mBall)
    {
        auto& cpBall = mBall.getComponent<CPhysics>();
        auto& cpBrick = mBrick.getComponent<CPhysics>();

        if (!CMath::isIntersecting(cpBrick, cpBall)) return;

        mBrick.destroy();

        // test collision scenario to deduce reaction
        float overlapLeft{ cpBall.right() - cpBrick.left() };
        float overlapRight{ cpBrick.right() - cpBall.left() };
        float overlapTop{ cpBall.bottom() - cpBrick.top() };
        float overlapBottom{ cpBrick.bottom() - cpBall.top() };

        bool BallFromLeft = abs(overlapLeft) < abs(overlapRight);
        bool BallFromTop = abs(overlapTop) < abs(overlapBottom);

        float minOverlapX{ BallFromLeft ? overlapLeft : overlapRight };
        float minOverlapY{ BallFromTop ? overlapTop : overlapBottom };

        // deduce if ball repel horizontally or vertically
        if (abs(minOverlapX) < abs(minOverlapY))
            cpBall.velocity.x = BallFromLeft ? -ballVelocity : ballVelocity;
        else
            cpBall.velocity.y = BallFromTop ? -ballVelocity : ballVelocity;
    }
}
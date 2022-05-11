#pragma once

#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "Arkanoid_ECS.h"
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
    { _context->render(shape); }

    void CRectangle::draw()
    {
        _context->render(shape);
    }
}
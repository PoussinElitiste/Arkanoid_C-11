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
    CPosition::CPosition(Entity &entity, const CVect2 &position)
        : Component(entity), _position{ position }
    {}

    void CPosition::IncPos(const CVect2& dir)
    {
        _position += dir;
    }

    CPhysics::CPhysics(Entity& entity, const CVect2& mHalfSize)
        : Component(entity), _halfSize{ mHalfSize } {}

    void CPhysics::Init()
    {
    }

    CPhysics& CPhysics::Velocity(const CVect2&& velocity)
    {
        _velocity = velocity;
        return *this;
    }

    CPhysics& CPhysics::Callback(Vect2Callback cb)
    {
        _onOutOfBounds = cb;
        return *this;
    }

    void CPhysics::Update(Frametime mFT)
    {
        _entity.getComponent<CPosition>().IncPos(_velocity * mFT);

        if (_onOutOfBounds == nullptr) return;

        if (left() < 0)	_onOutOfBounds(CVect2{ 1.f, 0.f });
        else if (right() > windowWidth)	_onOutOfBounds(CVect2{ -1.f, 0.f });

        if (top() < 0) _onOutOfBounds(CVect2{ 0.f, 1.f });
        else if (bottom() > windowHeight) _onOutOfBounds(CVect2{ 0.f, -1.f });
    }

    float CPhysics::left() const noexcept
    {
        return Position().x - _halfSize.x;
    }

    float CPhysics::right() const noexcept
    {
        return Position().x + _halfSize.x;
    }

    float CPhysics::top() const noexcept
    {
        return Position().y - _halfSize.y;
    }

    float CPhysics::bottom() const noexcept
    {
        return Position().y + _halfSize.y;
    }

    void CCircle::Init()
    {
        shape.setRadius(ballRadius);
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(ballRadius, ballRadius);
    }

    CCircle& CCircle::Color(sf::Color mColor)
    {
        shape.setFillColor(mColor);
        return *this;
    }

    void CCircle::Update(Frametime)
    {
        shape.setPosition(_entity.getComponent<CPosition>().Get());
    }

    void CCircle::Draw()
    { 
        _context->render(shape); 
    }

    void CRectangle::Draw()
    {
        _context->render(shape);
    }
}
#pragma once

#include "Arkanoid_ECS.h"

#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "CMath.h"
#include "Game.h"
#include "Entity.h"

using namespace ECS;

namespace Arkanoid
{
    CPosition::CPosition(Entity& entity, const CVect2& position)
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

    void CPhysics::Update(Frametime ft)
    {
        _entity.getComponent<CPosition>().IncPos(_velocity * ft);

        if (_onOutOfBounds == nullptr) return;

        if (left() < 0)	_onOutOfBounds(CVect2{ 1.f, 0.f });
        else if (right() > windowWidth)	_onOutOfBounds(CVect2{ -1.f, 0.f });

        if (top() < 0) _onOutOfBounds(CVect2{ 0.f, 1.f });
        else if (bottom() > windowHeight) _onOutOfBounds(CVect2{ 0.f, -1.f });
    }

    inline const CVect2& CPhysics::Position() const noexcept
    {
        return _entity.getComponent<CPosition>().Get();
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
        _shape.setRadius(ballRadius);
        _shape.setFillColor(sf::Color::Red);
        _shape.setOrigin(ballRadius, ballRadius);
    }

    CCircle::CCircle(Entity& entity, Game* context, float radius)
        : Component(entity), _context{ context }, _radius{ radius }{}

    CCircle& CCircle::Color(sf::Color mColor)
    {
        _shape.setFillColor(mColor);
        return *this;
    }

    void CCircle::Update(Frametime)
    {
        _shape.setPosition(_entity.getComponent<CPosition>().Get());
    }

    void CCircle::Draw()
    { 
        _context->render(_shape); 
    }

    void CRectangle::Init()
    {
        _shape.setSize({ paddleWidth, paddleHeight });
        _shape.setFillColor(sf::Color::Red);
        _shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
    }

    CRectangle::CRectangle(Entity& entity, Game* context)
        : Component(entity), _context{ context } {}

    CRectangle& CRectangle::Color(sf::Color mColor)
    {
        _shape.setFillColor(mColor);
        return *this;
    }

    CRectangle& CRectangle::Size(const CVect2& size)
    {
        _shape.setSize(size);
        return *this;
    }

    void CRectangle::Update(Frametime)
    {
        _shape.setPosition(_entity.getComponent<CPosition>().Get());
    }

    void CRectangle::Draw()
    {
        _context->render(_shape);
    }

    CPaddleControl::CPaddleControl(Entity& entity)
        : Component(entity) {}

    void CPaddleControl::Update(Frametime)
    {
        CPhysics& item = _entity.getComponent<CPhysics>();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && item.left() > 0)
            item.Velocity({ -paddleVelocity, item.Velocity().y });
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && item.right() < windowWidth)
            item.Velocity({ paddleVelocity, item.Velocity().y });
        else if (item.Velocity().x != 0.f)
            item.Velocity({ {}, item.Velocity().y });
    }
}
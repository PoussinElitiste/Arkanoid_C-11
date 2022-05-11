#pragma once

#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "CMath.h"
#include "Component.h"
#include "Entity.h"
#include "Observer.h"
#include "System.h"

using namespace ECS;

namespace Arkanoid
{
	// Game implementation
	class Game;

	// EC version
	//------------
	struct CPosition : Component
	{
		sf::Vector2f _position;

		// we assume root position is the center of the shape
        CPosition(Entity &entity, const sf::Vector2f &position);

		float x() const noexcept { return _position.x; }
		float y() const noexcept { return _position.y; }
	};

    struct CPhysics : Component
	{
		sf::Vector2f velocity, halfSize;

		std::function<void(const sf::Vector2f &)> onOutOfBounds;

		CPhysics(Entity& entity, const sf::Vector2f &mHalfSize) : Component(entity), halfSize{ mHalfSize } {}

		void init() override
		{
		}

		CPhysics &setVelocity(const sf::Vector2f &mVelocity)
		{
			velocity = mVelocity;

			return *this;
		}

		void update(Frametime mFT) override
		{
			_entity.getComponent<CPosition>()._position += velocity * mFT;

			if (onOutOfBounds == nullptr) return;

			if (left() < 0)	onOutOfBounds(sf::Vector2f{ 1.f, 0.f });
			else if (right() > windowWidth)	onOutOfBounds(sf::Vector2f{ -1.f, 0.f });

			if (top() < 0) onOutOfBounds(sf::Vector2f{ 0.f, 1.f });
			else if (bottom() > windowHeight) onOutOfBounds(sf::Vector2f{ 0.f, -1.f });
		}

		float x() const noexcept 
		{ 
			return _entity.getComponent<CPosition>().x();
		}

		float y() const noexcept 
		{
			return _entity.getComponent<CPosition>().y();
		}

		float left()	const noexcept { return x() - halfSize.x; }
		float right()	const noexcept { return x() + halfSize.x; }
		float top()		const noexcept { return y() - halfSize.y; }
		float bottom()	const noexcept { return y() + halfSize.y; }
	};

	struct CCircle : Component
	{
		// TODO: use DIP injection
		Game* _context = {};

		// define the composition itself
		sf::CircleShape shape;
		float radius;

		CCircle(Entity& entity, Game *mGame, float mRadius)
			: Component(entity), _context{ mGame }, radius{ mRadius }{}

		void init() override
		{
			shape.setRadius(ballRadius);
			shape.setFillColor(sf::Color::Red);
			shape.setOrigin(ballRadius, ballRadius);
		}

		CCircle &setColor(sf::Color mColor)
		{
			shape.setFillColor(mColor);
			return *this;
		}

		void update(Frametime) override
		{
			shape.setPosition(_entity.getComponent<CPosition>()._position);
		}

		// defined after Game class
		void draw() override;
	};

	struct CRectangle : Component
	{
		// TODO: use DIP injection
		Game* _context = {};

		// define the composition itself
		sf::RectangleShape shape;

		CRectangle(Entity& entity, Game* context)
			: Component(entity), _context{ context } {}

		void init() override
		{
			shape.setSize({ paddleWidth, paddleHeight });
			shape.setFillColor(sf::Color::Red);
			shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
		}

		CRectangle &setColor(sf::Color mColor)
		{
			shape.setFillColor(mColor);
			return *this;
		}

		CRectangle &setSize(const sf::Vector2f size)
		{
			shape.setSize(size);
			return *this;
		}

		void update(Frametime) override
        {
            shape.setPosition(_entity.getComponent<CPosition>()._position);
		}

		// defined after Game class
		void draw() override;
	};

	struct CPaddleControl : Component
	{
        CPaddleControl(Entity &entity) : Component(entity) {}

		void update(Frametime) override
		{
			auto& tmp = _entity.getComponent<CPhysics>();
			
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && tmp.left() > 0)
                tmp.velocity.x = -paddleVelocity;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && tmp.right() < windowWidth)
                tmp.velocity.x = paddleVelocity;
            else if (tmp.velocity.x != 0.f)
                tmp.velocity.x = {};
			
		}
	};
}
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
	class CPosition : public Component
	{
		CVect2 _position;
    public:
		// we assume root position is the center of the shape
        CPosition(Entity &entity, const CVect2 &position);

        void IncPos(const CVect2& dir);
        inline const CVect2& Get() const noexcept { return _position; }
	};

    class CPhysics : public Component
	{
		CVect2 _velocity, _halfSize;

        Vect2Callback _onOutOfBounds;

    public:
		CPhysics(Entity& entity, const CVect2 &mHalfSize);

		void Init() override;

        CPhysics& Velocity(const CVect2&& velocity);
        CPhysics& Callback(Vect2Callback cb);

        inline const CVect2& Velocity() const noexcept 
        { return _velocity; }
		void Update(Frametime mFT) override;

		inline const CVect2& Position() const noexcept 
        { return _entity.getComponent<CPosition>().Get(); }

		float left()	const noexcept;
		float right()	const noexcept;
		float top()		const noexcept;
		float bottom()	const noexcept;
	};

	class CCircle : public Component
	{
		// TODO: use DIP injection
		Game* _context = {};

		// define the composition itself
		sf::CircleShape shape;
		float radius;
    public:
		CCircle(Entity& entity, Game *mGame, float mRadius)
			: Component(entity), _context{ mGame }, radius{ mRadius }{}

		CCircle& Color(sf::Color mColor);

        void Init() override;
		void Update(Frametime) override;
		void Draw() override;
	};

	struct CRectangle : public Component
	{
		// TODO: use DIP injection
		Game* _context = {};

		// define the composition itself
		sf::RectangleShape shape;

		CRectangle(Entity& entity, Game* context)
			: Component(entity), _context{ context } {}

		void Init() override
		{
			shape.setSize({ paddleWidth, paddleHeight });
			shape.setFillColor(sf::Color::Red);
			shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
		}

		CRectangle &Color(sf::Color mColor)
		{
			shape.setFillColor(mColor);
			return *this;
		}

		CRectangle &setSize(const CVect2 size)
		{
			shape.setSize(size);
			return *this;
		}

		void Update(Frametime) override
        {
            shape.setPosition(_entity.getComponent<CPosition>().Get());
		}

		// defined after Game class
		void Draw() override;
	};

	struct CPaddleControl : public Component
	{
        CPaddleControl(Entity &entity) : Component(entity) {}

		void Update(Frametime) override
		{
            CPhysics& item = _entity.getComponent<CPhysics>();
			
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && item.left() > 0)
                item.Velocity({-paddleVelocity, item.Velocity().y});
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && item.right() < windowWidth)
                item.Velocity({paddleVelocity, item.Velocity().y});
            else if (item.Velocity().x != 0.f)
                item.Velocity({{}, item.Velocity().y});
		}
	};
}
#pragma once

#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "Component.h"

using namespace ECS;

namespace Arkanoid
{
    class Game;

	class CPosition : public Component
	{
		CVect2 _position;
    public:
		// we assume root position is the center of the shape
        CPosition(Entity& entity, const CVect2& position);

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

        inline const CVect2& Velocity() const noexcept { return _velocity; }
		void Update(Frametime ft) override;
		inline const CVect2& Position() const noexcept;

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
		sf::CircleShape _shape;
		float _radius;
    public:
		CCircle(Entity& entity, Game* context, float radius);
		CCircle& Color(sf::Color mColor);

        void Init() override;
		void Update(Frametime) override;
		void Draw() override;
	};

	class CRectangle : public Component
	{
		// TODO: use DIP injection
		Game* _context = {};
		sf::RectangleShape _shape;

    public:
		CRectangle(Entity& entity, Game* context);

		CRectangle& Color(sf::Color mColor);
		CRectangle& Size(const CVect2& size);

        void Init() override;
        void Update(Frametime) override;
		void Draw() override;
	};

	class CPaddleControl: public Component
	{
    public:
        CPaddleControl(Entity& entity);
		void Update(Frametime) override;
	};
}
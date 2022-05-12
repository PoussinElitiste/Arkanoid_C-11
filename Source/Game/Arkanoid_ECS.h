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

	class CRectangle : public Component
	{
		// TODO: use DIP injection
		Game* _context = {};

		// define the composition itself
		sf::RectangleShape shape;

    public:
		CRectangle(Entity& entity, Game* context);

		CRectangle& Color(sf::Color mColor);
		CRectangle& Size(const CVect2& size);

        void Init() override;
        void Update(Frametime) override;
		void Draw() override;
	};

	class CPaddleControl : public Component
	{
    public:
        CPaddleControl(Entity &entity);
		void Update(Frametime) override;
	};
}
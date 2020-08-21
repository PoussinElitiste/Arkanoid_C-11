#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "Core/PhysicUtils.h"
#include "Core/Component.h"
#include "Core/Entity.h"
#include "Core/Manager.h"

using namespace std;
using namespace sf;
using namespace ECS;
using namespace PhysicUtils;

namespace Arkanoid
{
	// Game implementation
	struct Game_v2;

	// EC version
	//------------
	struct CPosition : Component
	{
		Vector2f _position;

		// we assume root position is the center of the shape
        CPosition(Entity &entity, const Vector2f &mPosition);

		float x() const noexcept { return _position.x; }
		float y() const noexcept { return _position.y; }
	};

    CPosition::CPosition(Entity &entity, const Vector2f &mPosition)
        : Component(entity), _position{ mPosition }
    {}

    struct CPhysics : Component
	{
		std::weak_ptr<CPosition> cPosition;
		Vector2f velocity, halfSize;

		function<void(const Vector2f &)> onOutOfBounds;

		CPhysics(Entity& entity, const Vector2f &mHalfSize) : Component(entity), halfSize{ mHalfSize } {}

		void init() override
		{
			cPosition = _entity.getComponent<CPosition>();
		}

		CPhysics &setVelocity(const Vector2f &mVelocity)
		{
			velocity = mVelocity;

			return *this;
		}

		void update(Frametime mFT) override
		{
			if (auto tmp = cPosition.lock())
				tmp->_position += velocity * mFT;

			if (onOutOfBounds == nullptr) return;

			if (left() < 0)	onOutOfBounds(Vector2f{ 1.f, 0.f });
			else if (right() > windowWidth)	onOutOfBounds(Vector2f{ -1.f, 0.f });

			if (top() < 0) onOutOfBounds(Vector2f{ 0.f, 1.f });
			else if (bottom() > windowHeight) onOutOfBounds(Vector2f{ 0.f, -1.f });
		}

		float x()		const noexcept 
		{ 
			if (auto tmp = cPosition.lock())
				return tmp->x();
			return 0.f;
		}
		float y()		const noexcept 
		{ 
			if (auto tmp = cPosition.lock())
				return tmp->y();
			return 0.f;
		}
		float left()	const noexcept { return x() - halfSize.x; }
		float right()	const noexcept { return x() + halfSize.x; }
		float top()		const noexcept { return y() - halfSize.y; }
		float bottom()	const noexcept { return y() + halfSize.y; }
	};

	struct CCircle : Component
	{
		// TODO: use DIP injection
		Game_v2 *game{ nullptr };
		std::weak_ptr<CPosition> cPosition;

		// define the composition itself
		CircleShape shape;
		float radius;

		CCircle(Entity& entity, Game_v2 *mGame, float mRadius)
			: Component(entity), game{ mGame }, radius{ mRadius }{}

		void init() override
		{
			cPosition = _entity.getComponent<CPosition>();

			shape.setRadius(ballRadius);
			shape.setFillColor(Color::Red);
			shape.setOrigin(ballRadius, ballRadius);
		}

		CCircle &setColor(Color mColor)
		{
			shape.setFillColor(mColor);
			return *this;
		}
        

		void update(Frametime) override
		{
			if (auto tmp = cPosition.lock())
				shape.setPosition(tmp->_position);
		}

		// defined after Game class
		void draw() override;
	};

	struct CRectangle : Component
	{
		// TODO: use DIP injection
		Game_v2 *game{ nullptr };
		std::weak_ptr<CPosition> cPosition;

		// define the composition itself
		RectangleShape shape;

		CRectangle(Entity& entity, Game_v2 *mGame)
			: Component(entity), game{ mGame } {}

		void init() override
		{
			cPosition = _entity.getComponent<CPosition>();

			shape.setSize({ paddleWidth, paddleHeight });
			shape.setFillColor(Color::Red);
			shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
		}

		CRectangle &setColor(Color mColor)
		{
			shape.setFillColor(mColor);
			return *this;
		}

		CRectangle &setSize(const Vector2f size)
		{
			shape.setSize(size);
			return *this;
		}

		void update(Frametime) override
		{
			if (auto tmp = cPosition.lock())
				shape.setPosition(tmp->_position);
		}

		// defined after Game class
		void draw() override;
	};

	struct CPaddleControl : Component
	{
		std::weak_ptr<CPhysics> cPhysics;

        CPaddleControl(Entity &entity) : Component(entity) {}

		void init() override
		{
			cPhysics = _entity.getComponent<CPhysics>();
		}

		void update(Frametime) override
		{
			if (auto tmp = cPhysics.lock())
			{
				if (Keyboard::isKeyPressed(Keyboard::Key::Left) && tmp->left() > 0)
					tmp->velocity.x = -paddleVelocity;
				else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && tmp->right() < windowWidth)
					tmp->velocity.x = paddleVelocity;
				else if (tmp->velocity.x != 0.f)
					tmp->velocity.x = {};
			}
		}
	};

	void processCollisionPB(Entity &mPaddle, Entity &mBall)
	{
        if (auto ballPtr = mBall.getComponent<CPhysics>().lock())
            if (auto paddlePtr = mPaddle.getComponent<CPhysics>().lock())
            {
                auto &cpBall{*ballPtr};
                auto &cpPaddle{*paddlePtr};
                if (!isIntersecting(cpPaddle, cpBall)) return;
                cpBall.velocity.y = -ballVelocity;
                if (cpBall.x() < cpPaddle.x()) cpBall.velocity.x = -ballVelocity;
                else cpBall.velocity.x = ballVelocity;
            }
	}

	void processCollisionBB(Entity &mBrick, Entity &mBall)
	{
        if (auto ballPtr = mBall.getComponent<CPhysics>().lock())
            if (auto paddlePtr = mBrick.getComponent<CPhysics>().lock())
            {
                auto &cpBall{ *ballPtr };
                auto &cpBrick{ *paddlePtr };

                if (!isIntersecting(cpBrick, cpBall)) return;

                mBrick.destroy();

                // test collision scenario to deduce reaction
                float overlapLeft { cpBall.right() - cpBrick.left() };
                float overlapRight { cpBrick.right() - cpBall.left() };
                float overlapTop { cpBall.bottom() - cpBrick.top() };
                float overlapBottom { cpBrick.bottom() - cpBall.top() };

                bool BallFromLeft = abs(overlapLeft) < abs(overlapRight);
                bool BallFromTop = abs(overlapTop) < abs(overlapBottom);

                float minOverlapX { BallFromLeft ? overlapLeft : overlapRight };
                float minOverlapY { BallFromTop ? overlapTop : overlapBottom };

                // deduce if ball repel horizontally or vertically
                if (abs(minOverlapX) < abs(minOverlapY))
                    cpBall.velocity.x = BallFromLeft ? -ballVelocity : ballVelocity;
                else
                    cpBall.velocity.y = BallFromTop ? -ballVelocity : ballVelocity;
            }
	}

	struct Game_v2
	{
		// we define group to accelerate testing
		enum ArkanoidGroup : std::size_t
		{
			GPaddle,
			GBrick,
			GBall
		};

		RenderWindow window{ { windowWidth, windowHeight }, "Arkanoid - components" };

		Frametime lastFt{ 0.f };

		// our frame counter
		Frametime currentSlice{ 0.f };

		bool running{ false };

		// handle all entities
		Manager manager;

		// factory
		Entity &createBall()
		{
			auto &entity(manager.addEntity());

			entity.addComponent<CPosition>(entity, Vector2f{ windowWidth / 2.f, windowHeight / 2.f });
			entity.addComponent<CCircle>(entity, this, ballRadius).setColor(Color::White);
            entity.addComponent<CPhysics>(entity, Vector2f{ ballRadius, ballRadius })
				.setVelocity(Vector2f{ -ballVelocity, -ballVelocity })
				// we delegate collision process to Game 
				.onOutOfBounds = [&entity](const Vector2f &mSide)
			{
                if (auto physicsPtr = entity.getComponent<CPhysics>().lock())
                {
                    auto &cPhysics { *physicsPtr };
                    if (mSide.x != 0.f)
                        cPhysics.velocity.x = abs(cPhysics.velocity.x) * mSide.x;

                    if (mSide.y != 0.f)
                        cPhysics.velocity.y = abs(cPhysics.velocity.y) * mSide.y;
                }
			};

			entity.addGroup(ArkanoidGroup::GBall);

			return entity;
		}

		Entity &createBrick(const Vector2f &position)
		{
			Vector2f halfSize{ blockWidth / 2.f, blockHeight / 2.f };
			auto &entity = manager.addEntity();

			entity.addComponent<CPosition>(entity, position);
			entity.addComponent<CPhysics>(entity, halfSize);
			entity.addComponent<CRectangle>(entity, this).setColor(Color::Yellow);

			entity.addGroup(ArkanoidGroup::GBrick);

			return entity;
		}

		Entity &createPaddle()
		{
			Vector2f halfSize{ paddleWidth / 2.f, paddleHeight / 2.f };
			auto &entity(manager.addEntity());

			entity.addComponent<CPosition>(entity, Vector2f{ windowWidth / 2.f, windowHeight - 60.f });
		    entity.addComponent<CPhysics>(entity, halfSize);
			entity.addComponent<CRectangle>(entity, this).setSize({ paddleWidth *1.5f, paddleHeight * 0.5f });
			entity.addComponent<CPaddleControl>(entity);

			entity.addGroup(ArkanoidGroup::GPaddle);

			return entity;
		}

		Game_v2()
		{
			// if fps are too slow, velocity process could skip collision
			window.setFramerateLimit(60);

			createPaddle();
			createBall();
			for (int iX{ 0 }; iX < countBlocksX; ++iX)
				for (int iY{ 0 }; iY < countBlocksY; ++iY)
					createBrick(Vector2f{ (iX + 1)*(blockWidth + 3) + 22, (iY + 1)*(blockHeight + 3) });
		}

		void run()
		{
			running = true;

			while (running)
			{
				auto timePoint1(chrono::high_resolution_clock::now());
				window.clear(Color::Black);

				inputPhase();
				updatePhase();
				drawPhase();

				auto timePoint2(chrono::high_resolution_clock::now());

				auto elapseTime(timePoint2 - timePoint1);

				Frametime ft{ chrono::duration_cast<chrono::duration<float, milli>>(elapseTime).count() };

				lastFt = ft;

				auto ftSeconds(ft / 1000.f);
				auto fps(1.f / ftSeconds);

				window.setTitle("FT: " + to_string(ft) + "\tFPS" + to_string(fps));
			}
		}
		void inputPhase()
		{
			// SFML tips: prevent window freezing
			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
				{
					window.close();
					break;
				}
			}

			if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) running = false;
		}

		void updatePhase()
		{
			currentSlice += lastFt;

			// handle fixed FPS independent from CPU clock
			// note : 
			// if process took too much time --> execute several time the frame
			// if process took too less time --> skip the frame
			for (; currentSlice >= ftSlice; currentSlice -= ftSlice)
			{
				manager.refresh();
				// element must be update at fixed time to get precision
				manager.update(ftStep);

				auto &paddles(manager.getEntitiesByGroup(GPaddle));
				auto &bricks(manager.getEntitiesByGroup(GBrick));
				auto &balls(manager.getEntitiesByGroup(GBall));

				for (auto &ball : balls)
				{
					for (auto &paddle : paddles)
						processCollisionPB(*paddle, *ball);

					for (auto &brick : bricks)
						processCollisionBB(*brick, *ball);
				}
			}
		}

		void drawPhase()
		{
			manager.draw();
			window.display();
		}
		void render(const Drawable &mDrawable) { window.draw(mDrawable); }
	};

	void CCircle::draw() { game->render(shape); }
	void CRectangle::draw() { game->render(shape); }
}
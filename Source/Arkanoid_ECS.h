#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <bitset>
#include <array>
#include <assert.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "Core/ECS.h"
#include "Core/PhysicUtils.h"

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
		Vector2f position;
		CPosition() = default;

		// we assume root position is the center of the shape
		CPosition(const Vector2f &mPosition) : position{ mPosition } {}

		float x() const noexcept { return position.x; }
		float y() const noexcept { return position.y; }
	};

	struct CPhysics : Component
	{
		CPosition *cPosition{ nullptr };
		Vector2f velocity, halfSize;

		function<void(const Vector2f &)> onOutOfBounds;

		CPhysics(const Vector2f &mHalfSize) : halfSize{ mHalfSize } {}

		void init() override
		{
			cPosition = &entityPtr->getComponent<CPosition>();
		}

		CPhysics &setVelocity(const Vector2f &mVelocity)
		{
			velocity = mVelocity;

			return *this;
		}

		void update(Frametime mFT) override
		{
			cPosition->position += velocity * mFT;

			if (onOutOfBounds == nullptr) return;

			if (left() < 0)	onOutOfBounds(Vector2f{ 1.f, 0.f });
			else if (right() > windowWidth)	onOutOfBounds(Vector2f{ -1.f, 0.f });

			if (top() < 0) onOutOfBounds(Vector2f{ 0.f, 1.f });
			else if (bottom() > windowHeight) onOutOfBounds(Vector2f{ 0.f, -1.f });
		}

		float x()		const noexcept { return cPosition->x(); }
		float y()		const noexcept { return cPosition->y(); }
		float left()	const noexcept { return x() - halfSize.x; }
		float right()	const noexcept { return x() + halfSize.x; }
		float top()		const noexcept { return y() - halfSize.y; }
		float bottom()	const noexcept { return y() + halfSize.y; }
	};

	struct CCircle : Component
	{
		// TODO: use DIP injection
		Game_v2 *game{ nullptr };
		CPosition *cPosition{ nullptr };

		// define the composition itself
		CircleShape shape;
		float radius;

		CCircle(Game_v2 *mGame, float mRadius)
			: game{ mGame }, radius{ mRadius }{}

		void init() override
		{
			cPosition = &entityPtr->getComponent<CPosition>();

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
			shape.setPosition(cPosition->position);
		}

		// defined after Game class
		void draw() override;
	};

	struct CRectangle : Component
	{
		// TODO: use DIP injection
		Game_v2 *game{ nullptr };
		CPosition *cPosition{ nullptr };

		// define the composition itself
		RectangleShape shape;

		CRectangle(Game_v2 *mGame)
			: game{ mGame } {}

		void init() override
		{
			cPosition = &entityPtr->getComponent<CPosition>();

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
			shape.setPosition(cPosition->position);
		}

		// defined after Game class
		void draw() override;
	};

	struct CPaddleControl : Component
	{
		CPhysics *cPhysics{ nullptr };

		void init() override
		{
			cPhysics = &entityPtr->getComponent<CPhysics>();
		}

		void update(Frametime)
		{
			if (Keyboard::isKeyPressed(Keyboard::Key::Left) && cPhysics->left() > 0)
				cPhysics->velocity.x = -paddleVelocity;
			else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && cPhysics->right() < windowWidth)
				cPhysics->velocity.x = paddleVelocity;
			else if (cPhysics->velocity.x != 0.f)
				cPhysics->velocity.x = {};
		}
	};

	void processCollisionPB(Entity &mPaddle, Entity &mBall)
	{
		auto &cpPaddle(mPaddle.getComponent<CPhysics>());
		auto &cpBall(mBall.getComponent<CPhysics>());

		if (!isIntersecting(cpPaddle, cpBall)) return;

		cpBall.velocity.y = -ballVelocity;
		if (cpBall.x() < cpPaddle.x()) cpBall.velocity.x = -ballVelocity;
		else cpBall.velocity.x = ballVelocity;
	}

	void processCollisionBB(Entity &mBrick, Entity &mBall)
	{
		auto &cpBrick(mBrick.getComponent<CPhysics>());
		auto &cpBall(mBall.getComponent<CPhysics>());

		if (!isIntersecting(cpBrick, cpBall)) return;

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

			entity.addComponent<CPosition>(Vector2f{ windowWidth / 2.f, windowHeight / 2.f });
			entity.addComponent<CCircle>(this, ballRadius).setColor(Color::White);
			entity.addComponent<CPhysics>(Vector2f{ ballRadius, ballRadius })
				.setVelocity(Vector2f{ -ballVelocity, -ballVelocity })
				// we delegate collision process to Game 
				.onOutOfBounds = [&entity](const Vector2f &mSide)
			{
				auto &cPhysics{ entity.getComponent<CPhysics>() };
				if (mSide.x != 0.f)
					cPhysics.velocity.x = abs(cPhysics.velocity.x) * mSide.x;

				if (mSide.y != 0.f)
					cPhysics.velocity.y = abs(cPhysics.velocity.y) * mSide.y;
			};

			entity.addGroup(ArkanoidGroup::GBall);

			return entity;
		}

		Entity &createBrick(const Vector2f &mPosition)
		{
			Vector2f halfSize{ blockWidth / 2.f, blockHeight / 2.f };
			auto &entity(manager.addEntity());

			entity.addComponent<CPosition>(mPosition);
			entity.addComponent<CPhysics>(halfSize);
			entity.addComponent<CRectangle>(this).setColor(Color::Yellow);

			entity.addGroup(ArkanoidGroup::GBrick);

			return entity;
		}

		Entity &createPaddle()
		{
			Vector2f halfSize{ paddleWidth / 2.f, paddleHeight / 2.f };
			auto &entity(manager.addEntity());

			entity.addComponent<CPosition>(Vector2f{ windowWidth / 2.f, windowHeight - 60.f });
			entity.addComponent<CPhysics>(halfSize);
			entity.addComponent<CRectangle>(this).setSize({ paddleWidth *1.5f, paddleHeight * 0.5f });
			entity.addComponent<CPaddleControl>();

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

			// handle fixed FPS independant from CPU clock
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
//
// basic implementation
//----------------------
#pragma once

#include "pch.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <bitset>
#include <array>
#include <cassert>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Arkanoid_Global.h"
#include "Physic.h"

namespace Arkanoid
{
	struct Ball
	{
		sf::CircleShape shape;
		sf::Vector2f velocity{ -ballVelocity, -ballVelocity };
		Ball(float mX, float mY)
		{
			shape.setPosition(mX, mY);
			shape.setRadius(ballRadius);
			shape.setFillColor(sf::Color::Red);
			shape.setOrigin(ballRadius, ballRadius);
		}

		void update(Frametime mFT)
		{
			shape.move(velocity * mFT);
			if (left() < 0)
			{
				velocity.x = ballVelocity;
			}
			else if (right() > windowWidth)
			{
				velocity.x = -ballVelocity;
			}

			if (top() < 0)
			{
				velocity.y = ballVelocity;
			}
			else if (bottom() > windowHeight)
			{
				velocity.y = -ballVelocity;
			}
		}
		// const needed -> no modifying function
		// noexcept -> compile optimisation if function never throw an exception -> quite safe if added by mistake
		float x()		const noexcept { return shape.getPosition().x; }
		float y()		const noexcept { return shape.getPosition().y; }
		float left()	const noexcept { return x() - shape.getRadius(); }
		float right()	const noexcept { return x() + shape.getRadius(); }
		float top()		const noexcept { return y() - shape.getRadius(); }
		float bottom()	const noexcept { return y() + shape.getRadius(); }
	};

	struct Rectangle
	{
		sf::RectangleShape shape;

		float x()		const noexcept { return shape.getPosition().x; }
		float y()		const noexcept { return shape.getPosition().y; }
		float left()	const noexcept { return x() - shape.getSize().x / 2; }
		float right()	const noexcept { return x() + shape.getSize().x / 2; }
		float top()		const noexcept { return y() - shape.getSize().y / 2; }
		float bottom()	const noexcept { return y() + shape.getSize().y / 2; }
	};

	struct Paddle : public Rectangle
	{
		sf::Vector2f velocity;
		Paddle(float mX, float mY)
		{
			shape.setPosition(mX, mY);
			shape.setSize({ paddleWidth, paddleHeight });
			shape.setFillColor(sf::Color::Cyan);
			shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
		}

		void update(Frametime mFT)
		{
			shape.move(velocity * mFT);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > 0)
			{
				velocity.x = -paddleVelocity;
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < windowWidth)
			{
				velocity.x = paddleVelocity;
			}
			else
			{
				velocity.x = {};
			}
		}
	};

	struct Brick : public Rectangle
	{
		sf::Vector2f velocity;

		bool destroyed{ false };

		Brick(float mX, float mY)
		{
			shape.setPosition(mX, mY);
			shape.setSize({ blockWidth, blockHeight });
			shape.setFillColor(sf::Color::Yellow);
			shape.setOrigin(blockWidth * .5f, blockHeight * .5f);
		}
	};

	void testCollisionPB(Paddle &mPaddle, Ball &mBall)
	{
		if (!Physic::isIntersecting(mPaddle, mBall))
		{
			return;
		}

		mBall.velocity.y = -ballVelocity;
		if (mBall.x() < mPaddle.x())
		{
			mBall.velocity.x = -ballVelocity;
		}
		else
		{
			mBall.velocity.x = ballVelocity;
		}
	}

	void testCollisionBB(Brick &mBrick, Ball &mBall)
	{
		if (!Physic::isIntersecting(mBrick, mBall))
		{
			return;
		}
		mBrick.destroyed = true;

		float overlapLeft{ mBall.right() - mBrick.left() };
		float overlapRight{ mBrick.right() - mBall.left() };
		float overlapTop{ mBall.bottom() - mBrick.top() };
		float overlapBottom{ mBrick.bottom() - mBall.top() };

		bool BallFromLeft = abs(overlapLeft) < abs(overlapRight);
		bool BallFromTop = abs(overlapTop) < abs(overlapBottom);

		float minOverlapX{ BallFromLeft ? overlapLeft : overlapRight };
		float minOverlapY{ BallFromTop ? overlapTop : overlapBottom };

		if (abs(minOverlapX) < abs(minOverlapY))
		{
			mBall.velocity.x = BallFromLeft ? -ballVelocity : ballVelocity;
		}
		else
		{
			mBall.velocity.y = BallFromTop ? -ballVelocity : ballVelocity;
		}
	}

	struct Game
	{
		sf::RenderWindow window{ { windowWidth, windowHeight }, "Arkanoid - 1" };

		Frametime lastFt{ 0.f };

		// our frame counter
		Frametime currentSlice{ 0.f };

		bool running{ false };
		std::vector<std::unique_ptr<Ball>> balls;
		std::vector<std::unique_ptr<Paddle>> paddles;
		std::vector<std::unique_ptr<Brick>> bricks;

		Game()
		{
			//const VideoMode windowSize(windowWidth, windowHeight);

			//lastFt = 5;
			//currentSlice += 5;

			// if fps are too slow, velocity process could skip collision
			//window.setFramerateLimit(240);
			//window.setFramerateLimit(60);
			window.setFramerateLimit(30);
			//window.setFramerateLimit(15);

			for (int iX{ 0 }; iX < countBlocksX; ++iX)
			{
				for (int iY{ 0 }; iY < countBlocksY; ++iY)
				{
					bricks.emplace_back(std::make_unique<Brick>((iX + 1)*(blockWidth + 3) + 22, (iY + 1)*(blockHeight + 3)));
				}
			}

			balls.emplace_back(std::make_unique<Ball>(windowWidth * 0.5f, windowHeight * 0.5f));
			paddles.emplace_back(std::make_unique<Paddle>(windowWidth * 0.5f, windowHeight - 50.f));
		}

		void run()
		{
			running = true;

			while (true)
			{
				auto timePoint1(std::chrono::high_resolution_clock::now());
				window.clear(sf::Color::Black);

				inputPhase();
				updatePhase();
				drawPhase();

				auto timePoint2(std::chrono::high_resolution_clock::now());

				auto elapseTime(timePoint2 - timePoint1);

				Frametime ft{ std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(elapseTime).count() };

				lastFt = ft;

				auto ftSeconds(ft / 1000.f);
				auto fps(1.f / ftSeconds);

				window.setTitle("FT: " + std::to_string(ft) + "\tFPS" + std::to_string(fps));
			}
		}
		void inputPhase()
		{
			// SFML tips: prevent window freezing
			sf::Event evt;
			while (window.pollEvent(evt))
			{
				if (evt.type == sf::Event::Closed)
				{
					window.close();
					break;
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				running = false;
			}
		}

		void updatePhase()
		{
			currentSlice += lastFt;

			for (; currentSlice >= ftSlice; currentSlice -= ftSlice)
			{
				// element must be update at fixed time to get precision
				for (auto &ball : balls)
				{
					ball->update(ftStep);
				}

				for (auto &paddle : paddles)
				{
					paddle->update(ftStep);
				}

				for (auto &ball : balls)
				{
					for (auto &paddle : paddles)
					{
						testCollisionPB(*paddle, *ball);
					}

					for (auto &brick : bricks)
					{
						testCollisionBB(*brick, *ball);
					}
				}

				// Note: remove_if sort list and push all destroyed brick at the end of the list
				//  -> update several time on the same frame
				bricks.erase(remove_if(begin(bricks), end(bricks), [](const std::unique_ptr<Brick> &mBrick) {
					return mBrick->destroyed;
				}), end(bricks));
			}
		}

		void drawPhase()
		{
			for (auto &ball : balls)
			{
				window.draw(ball->shape);
			}

			for (auto &paddle : paddles)
			{
				window.draw(paddle->shape);
			}

			for (auto &brick : bricks)
			{
				window.draw(brick->shape);
			}
			window.display();
		}
	};
}
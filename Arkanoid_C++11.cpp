// Arkanoid_C++11.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "pch.h"

#include <chrono>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

using Frametime = float;

constexpr unsigned int windowWidth{ 800 }, windowHeight{ 600 };
constexpr float ballRadius{ 10.f }, ballVelocity{ 8.f };
constexpr float paddleWidth{ 60.f }, paddleHeight{ 20.f }, paddleVelocity{ 6.f };
constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };

constexpr float ftStep{ 1.f }, ftSlice{ 1.f };

struct Ball
{
	CircleShape shape;
	Vector2f velocity{ -ballVelocity, -ballVelocity };
	Ball(float mX, float mY)
	{
		shape.setPosition(mX, mY);
		shape.setRadius(ballRadius);
		shape.setFillColor(Color::Red);
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
	float x()		const noexcept{ return shape.getPosition().x; }
	float y()		const noexcept{ return shape.getPosition().y; }
	float left()	const noexcept{ return x() - shape.getRadius(); }
	float right()	const noexcept{ return x() + shape.getRadius(); }
	float top()		const noexcept{ return y() - shape.getRadius(); }
	float bottom()	const noexcept{ return y() + shape.getRadius(); }
};

struct Rectangle
{
	RectangleShape shape;

	float x()		const noexcept { return shape.getPosition().x; }
	float y()		const noexcept { return shape.getPosition().y; }
	float left()	const noexcept { return x() - shape.getSize().x / 2; }
	float right()	const noexcept { return x() + shape.getSize().x / 2; }
	float top()		const noexcept { return y() - shape.getSize().y / 2; }
	float bottom()	const noexcept { return y() + shape.getSize().y / 2; }
};

struct Paddle: public Rectangle
{
	Vector2f velocity;
	Paddle(float mX, float mY)
	{
		shape.setPosition(mX, mY);
		shape.setSize({ paddleWidth, paddleHeight });
		shape.setFillColor(Color::Red);
		shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
	}

	void update(Frametime mFT)
	{
		shape.move(velocity * mFT);

		if (Keyboard::isKeyPressed(Keyboard::Key::Left)	&& left() > 0)
		{
			velocity.x = -paddleVelocity;
		}
		else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && right() < windowWidth)
		{
			velocity.x = paddleVelocity;
		}
		else
		{
			velocity.x = {};
		}
	}
};

struct Brick : Rectangle
{
	Vector2f velocity;
	
	bool destroyed{ false };

	Brick(float mX, float mY)
	{
		shape.setPosition(mX, mY);
		shape.setSize({ paddleWidth, paddleHeight });
		shape.setFillColor(Color::Yellow);
		shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
	}
};

template<class T1, class T2> bool isIntersecting(T1& mA, T2& mB)
{
	return mA.right() >= mB.left() && mA.left() <= mB.right() && mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Paddle &mPaddle, Ball &mBall)
{
	if (!isIntersecting(mPaddle, mBall))
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

void testCollision(Brick &mBrick, Ball &mBall)
{
	if (!isIntersecting(mBrick, mBall))
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
		mBall.velocity.x = BallFromLeft? -ballVelocity : ballVelocity;
	}
	else
	{
		mBall.velocity.y = BallFromTop ? -ballVelocity : ballVelocity;
	}
}

struct Game
{
	RenderWindow window{ { windowWidth, windowHeight }, "Arkanoid - 1" };

	Frametime lastFt{ 0.f };

	// our frame counter
	Frametime currentSlice{ 0.f };

	bool running{ false };
	Ball ball{ windowWidth / 2, windowHeight / 2 };
	Paddle paddle{ windowWidth / 2, windowHeight - 50 };
	vector<Brick> bricks;

	Game()
	{
		//const VideoMode windowSize(windowWidth, windowHeight);

		lastFt = 5;
		currentSlice += 5;

		// if fps are too slow, velocity process could skip collision
		//window.setFramerateLimit(240);
		//window.setFramerateLimit(60);
		window.setFramerateLimit(30);
		//window.setFramerateLimit(15);


		for (int iX{ 0 }; iX < countBlocksX; ++iX)
		{
			for (int iY{ 0 }; iY < countBlocksY; ++iY)
			{
				bricks.emplace_back((iX + 1)*(blockWidth + 3) + 22, (iY + 1)*(blockHeight + 3));
			}
		}
	}

	void run()
	{
		running = true;

		while (true)
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

		if (Keyboard::isKeyPressed(Keyboard::Key::Escape))
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
			ball.update(ftStep);
			paddle.update(ftStep);
			testCollision(paddle, ball);
			for (auto &brick : bricks)
			{
				testCollision(brick, ball);
			}

			// Note: remove_if sort list and push all destroyed brick at the end of the list
			//  -> update several time on the same frame
			bricks.erase(remove_if(begin(bricks), end(bricks), [](const Brick &mBrick) {
				return mBrick.destroyed;
			}), end(bricks));
		}
	}

	void drawPhase()
	{
		window.draw(ball.shape);
		window.draw(paddle.shape);

		for (auto &brick : bricks)
		{
			window.draw(brick.shape);
		}
		window.display();
	}
};

int main()
{
	Game{}.run();
	return 0;
}

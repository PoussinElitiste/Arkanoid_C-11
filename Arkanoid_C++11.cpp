// Arkanoid_C++11.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "pch.h"

#include <chrono>
#include <functional>
#include <iostream>
#include <bitset>
#include <array>
#include <assert.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

namespace Arkanoid
{
	// EC implementations
	struct Component;
	class Entity;
	class Manager;

	using ComponenID = std::size_t;
	using Group = std::size_t;

	namespace Internal
	{
		inline ComponenID getUniqueComponentID() noexcept
		{
			static ComponenID lastID{ 0u };
			return lastID++;
		}
	}	

	template<typename T> inline ComponenID getComponentTypeID() noexcept
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be inherit from Component");

		// will auto generate an unique type ID at compile time
		// note: template will generate "several" getComponentTypeID by Class -> unique static typeID by Class
		static ComponenID typeID{ Internal::getUniqueComponentID() };
		return typeID;
	}

	// max bit shift for 32 bit
	constexpr std::size_t maxComponents{ 32 };
	using ComponentBitset = std::bitset<maxComponents>;
	using ComponentArray = std::array<Component*, maxComponents>;

	constexpr std::size_t maxGroups{ 32 };
	using GroupBitset = std::bitset<maxGroups>; 

	using EntityList = std::vector<Entity*>;

	struct Component
	{
		Entity *entityPtr{ nullptr };

		virtual void init() {}
		virtual void update(float mFT) {}
		virtual void draw() {}
		virtual ~Component() {}
	};

	class Entity
	{
	private:
		Manager &manager;
		bool alive{ true };
		// warranty unique instance
		vector<unique_ptr<Component>> components;

		// keep a dictinnary to quick access component
		ComponentArray componentArray;

		// keep a flag table of component added -> unique component by entity
		ComponentBitset componentBitset;

		// used to define in shich group Entity is registered
		GroupBitset groupBitset;

	public:
		Entity(Manager &mManager) : manager(mManager) {}
		void update(float mFT)	{ for (auto &c : components) c->update(mFT); }
		void draw()				{ for (auto &c : components) c->draw(); }

		bool isAlive() const	{ return alive; }
		void destroy()			{ alive = false; }

		template<typename T> bool hasComponent() const
		{
			return componentBitset[getComponentTypeID<T>()];
		}

		bool hasGroup(Group mGroup) const noexcept
		{
			return groupBitset[mGroup];
		}

		// will register Entity group in manager
		// note: defined after manager class
		void addGroup(Group mGroup) noexcept;
		void delGroup(Group mGroup) noexcept
		{
			// will be used by manager during refresh
			groupBitset[mGroup] = false;
		}

		template<typename T, typename... TArgs>
			T& addComponent(TArgs &&... mArgs)
		{
			// only one component type by entity 
			assert(!hasComponent<T>());

			T* c(new T(forward<TArgs>(mArgs)...));

			c->entityPtr = this;

			// wrap into unique ptr to avoid memory leak with vector list
			unique_ptr<Component> uPtr{ c };
			
			// move is mandatory because unique_ptr cannot be copied
			components.emplace_back(move(uPtr));

			// register component -> could be 
			componentArray[getComponentTypeID<T>()] = c;
			componentBitset[getComponentTypeID<T>()] = true;

			c->init();
			return *c;
		}

		template<typename T>
		T &getComponent() const
		{
			assert(hasComponent<T>());
			auto ptr(componentArray[getComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
	};

	class Manager
	{
	private:
		vector<unique_ptr<Entity>> entities;

		// allow to register entities by groupID
		array<EntityList, maxGroups> groupedEntities;

	public:
		void update(float mFT)	{ for (auto &e : entities) e->update(mFT); }
		void draw()				{ for (auto &e : entities) e->draw(); }

		void addToGroup(Entity *mEntity, Group mGroup)
		{
			groupedEntities[mGroup].emplace_back(mEntity);
		}

		EntityList &getEntitiesByGroup(Group mGroup)
		{
			return groupedEntities[mGroup];
		}

		void refresh()
		{
			// remove from group first
			for (auto i{ 0u }; i < maxGroups; ++i)
			{
				auto &v(groupedEntities[i]);
				v.erase(
					remove_if(begin(v), end(v),
					[i](Entity *mEntity)
					{
						return !mEntity->isAlive() || !mEntity->hasGroup(i);
					}),
					end(v));
			}

			// Note: remove_if sort list and push all destroyed brick at the end of the list
			entities.erase(
				remove_if(begin(entities), end(entities),
				[](const unique_ptr<Entity> &mEntity)
				{
					return !mEntity->isAlive();
				}),
				end(entities));
		}

		Entity &addEntity()
		{
			Entity *e{ new Entity{*this} };
			// TODO: use DIP injection
			unique_ptr<Entity> uPtr{ e };
			entities.emplace_back(move(uPtr));

			return *e;
		}
	};

	// need to be defined after Manager
	void Entity::addGroup(Group mGroup) noexcept
	{
		groupBitset[mGroup] = true;
		manager.addToGroup(this, mGroup);
	}

	// Game implementation

	struct Game_v2;
	using Frametime = float;

	constexpr unsigned int windowWidth{ 800 }, windowHeight{ 600 };
	constexpr float ballRadius{ 10.f }, ballVelocity{ 8.f };
	constexpr float paddleWidth{ 60.f }, paddleHeight{ 20.f }, paddleVelocity{ 6.f };
	constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
	constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };

	// time base ref
	constexpr float ftStep{ 1.f }, ftSlice{ 1.f };

	template<class T1, class T2> bool isIntersecting(T1& mA, T2& mB) noexcept
	{
		return mA.right() >= mB.left() && mA.left() <= mB.right() && mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
	}

	// EC version
	//------------
	struct CPosition : Component
	{
		Vector2f position;
		CPosition() = default;

		// we assume root position is the center of the shape
		CPosition(const Vector2f &mPosition) : position{mPosition}{}

		float x() const noexcept { return position.x; }
		float y() const noexcept { return position.y; }
	};

	struct CPhysics : Component
	{
		CPosition *cPosition{ nullptr };
		Vector2f velocity, halfSize;

		function<void(const Vector2f &)> onOutOfBounds;

		CPhysics(const Vector2f &mHalfSize): halfSize{mHalfSize} {}

		void init() override
		{
			cPosition = &entityPtr->getComponent<CPosition>();
		}

		void update(Frametime mFT) override
		{
			cPosition->position += velocity * mFT;

			if (onOutOfBounds == nullptr) return;

			if (left() < 0)	onOutOfBounds(Vector2f{1.f, 0.f});
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

	struct CCircle: Component
	{
		// TODO: use DIP injection
		Game_v2 *game{ nullptr };
		CPosition *cPosition{ nullptr };

		// define the composition itself
		CircleShape shape;
		float radius;

		CCircle(Game_v2 *mGame, float mRadius) 
			: game{mGame}, radius{mRadius}{}

		void init() override
		{
			cPosition = &entityPtr->getComponent<CPosition>();

			shape.setRadius(ballRadius);
			shape.setFillColor(Color::Red);
			shape.setOrigin(ballRadius, ballRadius);
		}

		void update(Frametime mFT) override
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
			: game{ mGame }{}

		void init() override
		{
			cPosition = &entityPtr->getComponent<CPosition>();

			shape.setSize({ paddleWidth, paddleHeight });
			shape.setFillColor(Color::Red);
			shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
		}

		void update(Frametime mFT) override
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

		void update(Frametime mFT)
		{
			if (Keyboard::isKeyPressed(Keyboard::Key::Left) && cPhysics->left() > 0)
				cPhysics->velocity.x = -paddleVelocity;			
			else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && cPhysics->right() < windowWidth)
				cPhysics->velocity.x = paddleVelocity;
			else
				cPhysics->velocity.x = {};
		}
	};

	void processCollisionPB(Entity &mPaddle, Entity &mBall)
	{
		auto &cpPaddle(mPaddle.getComponent<CPhysics>());
		auto &cpBall(mBall.getComponent<CPhysics>());

		if (!isIntersecting(cpPaddle, cpBall)) return;

		cpBall.velocity.y = -ballVelocity;
		if (cpBall.x() < cpBall.x()) cpBall.velocity.x = -ballVelocity;
		else cpBall.velocity.x = ballVelocity;
	}

	void processCollisionBB(Entity &mBrick, Entity &mBall)
	{
		auto &cpBrick(mBrick.getComponent<CPhysics>());
		auto &cpBall(mBall.getComponent<CPhysics>());

		if (!isIntersecting(cpBrick, cpBall))
		{
			return;
		}
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
			entity.addComponent<CPhysics>(Vector2f{ ballRadius, ballRadius });
			entity.addComponent<CCircle>(this, ballRadius);

			// post initialisation
			auto cPhysics(entity.getComponent<CPhysics>());
			cPhysics.velocity = Vector2f{ -ballVelocity, -ballVelocity };

			// we delegate collision process to Game 
			cPhysics.onOutOfBounds = [&cPhysics](const Vector2f &mSide)
			{
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
			entity.addComponent<CRectangle>(this);

			entity.addGroup(ArkanoidGroup::GBrick);

			return entity;
		}

		Entity &createPaddle()
		{
			Vector2f halfSize{ paddleWidth / 2.f, paddleHeight/ 2.f };
			auto &entity(manager.addEntity());

			entity.addComponent<CPosition>(Vector2f{windowWidth/2.f, windowHeight - 60.f});
			entity.addComponent<CPhysics>(halfSize);
			entity.addComponent<CRectangle>(this);
			entity.addComponent<CPaddleControl>();

			entity.addGroup(ArkanoidGroup::GPaddle);

			return entity;
		}

		Game_v2()
		{
			// if fps are too slow, velocity process could skip collision
			window.setFramerateLimit(240);

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
			// note : if process took too much time we loop several time to "reach" lost time
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

	// basic implementation
	//----------------------
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

	struct Brick : public Rectangle
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

	void testCollisionPB(Paddle &mPaddle, Ball &mBall)
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

	void testCollisionBB(Brick &mBrick, Ball &mBall)
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
		vector<unique_ptr<Ball>> balls;
		//Ball ball{ windowWidth / 2, windowHeight / 2 };

		vector<unique_ptr<Paddle>> paddles;
		//Paddle paddle{ windowWidth / 2, windowHeight - 50 };

		vector<unique_ptr<Brick>> bricks;

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
					bricks.emplace_back(new Brick{ (iX + 1)*(blockWidth + 3) + 22, (iY + 1)*(blockHeight + 3) });
				}
			}

			balls.emplace_back(new Ball{ windowWidth / 2, windowHeight / 2 });
			paddles.emplace_back(new Paddle{ windowWidth / 2, windowHeight - 50 });
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
				bricks.erase(remove_if(begin(bricks), end(bricks), [](const unique_ptr<Brick> &mBrick) {
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

			for (auto &paddle : balls)
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

// Main
//------
using namespace Arkanoid;

int main()
{
	Game_v2{}.run();
	//Game{}.run();
	return 0;
}

#include "Game.h"
#include "Arkanoid_ECS.h"
#include "System.h"
#include "Entity.h"

using namespace ECS;

namespace Arkanoid
{
    Game::Game()
        : _window{ { windowWidth, windowHeight }, "Arkanoid - components" }
    {
        // if fps are too slow, velocity process could skip collision
        _window.setFramerateLimit(60);

        createPaddle();
        createBall();
        for (int iX{ 0 }; iX < countBlocksX; ++iX)
            for (int iY{ 0 }; iY < countBlocksY; ++iY)
                createBrick(sf::Vector2f{ (iX + 1) * (blockWidth + 3) + 22, (iY + 1) * (blockHeight + 3) });

        // TODO: create System
    }
        
    void Game::run()
    {
        _running = true;

        while (_running)
        {
            auto timePoint1(std::chrono::high_resolution_clock::now());
            _window.clear(sf::Color::Black);

            inputPhase();
            updatePhase();
            drawPhase();

            auto timePoint2(std::chrono::high_resolution_clock::now());

            auto elapseTime(timePoint2 - timePoint1);

            Frametime ft{ std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(elapseTime).count() };

            _lastFt = ft;

            auto ftSeconds(ft / 1000.f);
            auto fps(1.f / ftSeconds);

            _window.setTitle("FT: " + std::to_string(ft) + "\tFPS" + std::to_string(fps));
        }
    }

    void Game::inputPhase()
    {
        // SFML tips: prevent window freezing
        sf::Event event;
        while (_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                _window.close();
                break;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) _running = false;
    }

    void Game::updatePhase()
    {
        _currentSlice += _lastFt;

        // handle fixed FPS independent from CPU clock
        // note : 
        // if process took too much time --> execute several time the frame
        // if process took too less time --> skip the frame
        for (; _currentSlice >= ftSlice; _currentSlice -= ftSlice)
        {
            _manager.refresh();
            // element must be update at fixed time to get precision
            _manager.Update(ftStep);

            EntityList& paddles = _manager.getEntitiesByGroup(GPaddle);
            EntityList& bricks = _manager.getEntitiesByGroup(GBrick);
            EntityList& balls = _manager.getEntitiesByGroup(GBall);

            for (Entity* ball : balls)
            {
                for (Entity* paddle : paddles)
                    processCollisionPB(*paddle, *ball);

                for (Entity* brick : bricks)
                    processCollisionBB(*brick, *ball);
            }
        }
    }

    void Game::drawPhase()
    {
        _manager.Draw();
        _window.display();
    }

    void Game::render(const sf::Drawable& mDrawable)
    {
        _window.draw(mDrawable);
    }

    Entity& Game::createBall()
    {
        auto& entity = _manager.addEntity();

        entity.addComponent<CPosition>(entity, sf::Vector2f{ windowWidth / 2.f, windowHeight / 2.f });
        entity.addComponent<CCircle>(entity, this, ballRadius).Color(sf::Color::White);
        entity.addComponent<CPhysics>(entity, sf::Vector2f{ ballRadius, ballRadius })
            .Velocity(sf::Vector2f{ -ballVelocity, -ballVelocity })
            // we delegate collision process to Game 
            .Callback([&entity](const sf::Vector2f& side)
        {
            CPhysics& cp{ entity.getComponent<CPhysics>() };
            const CVect2& v = cp.Velocity();
            if (side.x != 0.f)
                cp.Velocity({abs(v.x) * side.x, v.y});

            if (side.y != 0.f)
                cp.Velocity({ v.x, abs(v.y) * side.y});
        });

        entity.addGroup(ArkanoidGroup::GBall);

        return entity;
    }

    Entity& Game::createBrick(const sf::Vector2f& position)
    {
        sf::Vector2f _halfSize{ blockWidth / 2.f, blockHeight / 2.f };
        auto& entity = _manager.addEntity();

        entity.addComponent<CPosition>(entity, position);
        entity.addComponent<CPhysics>(entity, _halfSize);
        entity.addComponent<CRectangle>(entity, this).Color(sf::Color::Yellow);

        entity.addGroup(ArkanoidGroup::GBrick);

        return entity;
    }

    Entity& Game::createPaddle()
    {
        sf::Vector2f _halfSize{ paddleWidth / 2.f, paddleHeight / 2.f };
        auto& entity(_manager.addEntity());

        entity.addComponent<CPosition>(entity, sf::Vector2f{ windowWidth / 2.f, windowHeight - 60.f });
        entity.addComponent<CPhysics>(entity, _halfSize);
        entity.addComponent<CRectangle>(entity, this).Size({ paddleWidth * 1.5f, paddleHeight * 0.5f });
        entity.addComponent<CPaddleControl>(entity);

        entity.addGroup(ArkanoidGroup::GPaddle);

        return entity;
    }

    System& Game::createSystem()
    {
        sf::Vector2f _halfSize{ paddleWidth / 2.f, paddleHeight / 2.f };
        auto& entity = _manager.addSystem<ECS::UpdateSystem>();

        return entity;
    }

    void Game::processCollisionPB(Entity& paddle, Entity& ball)
    {
        CPhysics& cpBall = ball.getComponent<CPhysics>();
        CPhysics& cpPaddle = paddle.getComponent<CPhysics>();

        const CVect2& vBall = cpBall.Velocity();
        const CVect2& vPaddle = cpPaddle.Velocity();

        const CVect2& pBall = cpBall.Position();
        const CVect2& pPaddle = cpPaddle.Position();

        if (!CMath::isIntersecting(cpPaddle, cpBall)) 
            return;

        if (pBall.x < pPaddle.x)
            cpBall.Velocity({ -ballVelocity, -ballVelocity });
        else 
            cpBall.Velocity({ ballVelocity, -ballVelocity });

    }

    void Game::processCollisionBB(Entity& brick, Entity& ball)
    {
        auto& cpBall = ball.getComponent<CPhysics>();
        auto& cpBrick = brick.getComponent<CPhysics>();

        if (!CMath::isIntersecting(cpBrick, cpBall)) return;

        brick.destroy();

        // test collision scenario to deduce reaction
        float overlapLeft = cpBall.right() - cpBrick.left();
        float overlapRight = cpBrick.right() - cpBall.left();
        float overlapTop = cpBall.bottom() - cpBrick.top();
        float overlapBottom = cpBrick.bottom() - cpBall.top();

        bool BallFromLeft = abs(overlapLeft) < abs(overlapRight);
        bool BallFromTop = abs(overlapTop) < abs(overlapBottom);

        float minOverlapX = BallFromLeft ? overlapLeft : overlapRight;
        float minOverlapY = BallFromTop ? overlapTop : overlapBottom;

        // deduce if ball repel horizontally or vertically
        if (abs(minOverlapX) < abs(minOverlapY))
            cpBall.Velocity({BallFromLeft ? -ballVelocity : ballVelocity, cpBall.Velocity().y});
        else
            cpBall.Velocity({ cpBall.Velocity().x, BallFromTop ? -ballVelocity : ballVelocity});
    }
}
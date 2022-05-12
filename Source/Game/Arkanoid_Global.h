#pragma once
#include <functional>

namespace Arkanoid
{
	constexpr unsigned int SCREEN_WIDTH{ 800 }, SCREEN_HEIGHT{ 600 };
	constexpr float BALL_RADIUS{ 7.f }, BALL_VELOCITY{ 0.4f };
	constexpr float PADDLE_WIDTH{ 80.f }, PADDLE_HEIGHT{ 20.f }, PADDLE_VELOCITY{ .6f };
	constexpr float BLOCK_WIDTH{ 60.f }, BLOCK_HEIGHT{ 20.f };
	constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };

	// time base ref
	constexpr float FT_STEP{ 1.f }, FT_SLICE{ 1.f };

	using Frametime = float;
    using uint = unsigned int;
    using CVect2 = sf::Vector2f;
    using Vect2Callback = std::function<void(const CVect2&)>;
}
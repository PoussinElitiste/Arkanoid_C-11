#pragma once
namespace Arkanoid
{
	constexpr unsigned int windowWidth{ 800 }, windowHeight{ 600 };
	constexpr float ballRadius{ 7.f }, ballVelocity{ 0.4f };
	constexpr float paddleWidth{ 80.f }, paddleHeight{ 20.f }, paddleVelocity{ .6f };
	constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
	constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };

	// time base ref
	constexpr float ftStep{ 1.f }, ftSlice{ 1.f };

	using Frametime = float;
    using uint = unsigned int;
}
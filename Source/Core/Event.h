#pragma once

#include <functional>

namespace Event
{
	using CallBack = std::function<void()>;

	struct Message
	{
		CallBack cb;
	};
}
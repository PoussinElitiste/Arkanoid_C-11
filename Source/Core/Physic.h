#pragma once

namespace Physic
{
	template<class T1, class T2> bool isIntersecting(T1& mA, T2& mB) noexcept
	{
		return mA.right() >= mB.left() && mA.left() <= mB.right() && mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
	}
}
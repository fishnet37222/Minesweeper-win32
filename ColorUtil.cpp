// Copyright (c) 2024 David A. Frischknecht
//
// SPDX-License-Identifier: Apache-2.0

#include "ColorUtil.h"

namespace
{
	[[nodiscard]] UINT8 AlphaBlend(const UINT8 fg, const UINT8 bg, const double alpha)
	{
		auto result = bg + alpha * (fg - bg);
		result = max(result, 0.0);
		result = min(result, 255.0);

		return static_cast<UINT8>(result);
	}
}

COLORREF ChangeColorLightness(const COLORREF source, UINT8 iAlpha)
{
	if (iAlpha == 100) return source;

	iAlpha = max(iAlpha, 0);
	iAlpha = min(iAlpha, 200);

	auto alpha = (iAlpha - 100.0) / 100.0;

	UINT8 bg;
	if (iAlpha > 100)
	{
		bg = 255;
		alpha = 1.0 - alpha;
	}
	else
	{
		bg = 0;
		alpha = 1.0 + alpha;
	}

	const auto r = AlphaBlend(GetRValue(source), bg, alpha);
	const auto g = AlphaBlend(GetGValue(source), bg, alpha);
	const auto b = AlphaBlend(GetBValue(source), bg, alpha);

	return RGB(r, g, b);
}

#pragma once

#include "../ds/Math.h"

namespace sz_gui 
{
	// frameUI数据
	struct FrameUIData
	{
		// 边框宽度
		float m_borderWidth = 1.0f;
		// 边框颜色
		sz_ds::Rgba4Byte m_color = sz_ds::Rgba4Byte(255, 255, 255, 255);
	};
}
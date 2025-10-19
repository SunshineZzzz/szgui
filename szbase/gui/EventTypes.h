#pragma once

#include <stdint.h>

#include "../ds/EventBus.h"

namespace sz_gui 
{
	namespace events
	{
        struct WindowEventData;
        using WindowEvent = sz_ds::Event<events::WindowEventData>;
        struct MouseButtonEventData;
        using MouseButtonEvent = sz_ds::Event<events::MouseButtonEventData>;

        // 窗口事件数据
        struct WindowEventData
        {
            enum class Type
            {
                // 窗口关闭
                QUIT,
            };
            // 窗口事件类型
            Type m_type;
        };

        // 鼠标点击事件数据
        struct MouseButtonEventData 
        {
            enum class Type
            {
                BUTTON_LEFT,
                BUTTON_RIGHT,
	            BUTTON_MIDDLE,
            };

            enum class State
            {
                UP,
				DOWN,
			};

            // 鼠标按键类型
            Type m_type;
            // 鼠标按键状态
            State m_state;
            // 相对于窗口x，y坐标
            float m_x{ 0.0f }, m_y{ 0.0f };
        };
	}
}
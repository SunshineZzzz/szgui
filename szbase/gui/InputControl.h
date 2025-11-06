// Comment: 输入控制

#pragma once

#include <unordered_map>
#include <any>

namespace sz_gui
{
	class InputControl
	{
	public:
		InputControl();
		virtual ~InputControl();

		// 鼠标按钮事件
		virtual void OnMouseButton(std::any eventContainer);
		// 鼠标移动事件
		virtual void OnMouseMotion(std::any eventContainer);

	public:
		// 鼠标左键是否按下
		bool m_mouseLeftIsDown = false;
		// 鼠标右键是否按下
		bool m_mouseRightIsDown = false;
		// 鼠标中键是否按下
		bool m_mouseMiddleIsDown = false;
		// 当前鼠标的位置
		float m_currentX = 0.0f;
		float m_currentY = 0.0f;
	};
}
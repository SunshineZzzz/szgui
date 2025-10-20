#pragma once

#include <string>
#include <tuple>

namespace sz_gui
{
	// 渲染接口
	class IRender
	{
	public:
		IRender() = default;
		virtual ~IRender() = default;
		
		IRender(const IRender&) = delete;
		IRender& operator=(const IRender&) = delete;

		IRender(IRender&&) = delete;
		IRender& operator=(IRender&&) = delete;

	public:
		// 带默认值设置清除颜色
		virtual bool SetClearColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) 
		{
			return SetClearColorImpl(r, g, b, a);
		}

	public:
		// 初始化
		virtual std::tuple<std::string, bool> Init() = 0;
		// 设置视口
		virtual bool SetViewPort(int,int,int,int) = 0;
		// 设置清除颜色
		virtual bool SetClearColorImpl(float, float, float, float) = 0;
		// 清除屏幕
		virtual bool Clear() = 0;
		// 交换缓冲区
		virtual bool SwapWindow() = 0;
	};
}
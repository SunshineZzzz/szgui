#pragma once

#include <SDL3/SDL.h>

#include <cstdint>
#include <string>
#include <tuple>
#include <memory>

#include "IRender.h"
#include "IUIManager.h"

namespace sz_gui 
{
	class SDLApp 
	{
	public:
		// 初始化SDL & ANGLE
		static std::tuple<std::string, bool> InitSDLWithANGLE();

	public:
		SDLApp();
		virtual ~SDLApp();

		// 创建窗口
		std::tuple<const std::string, bool> CreateWindow(
			const std::string& title, const int& width, const int& height);
		// Run循环
		void Run();
		// 注册顶层UI
		bool RegToUI(std::shared_ptr<IUIBase> ui);
		// 注销顶层UI
		bool UnRegTopUI(std::shared_ptr<IUIBase> ui);
		// 设置布局
		bool SetLayout(ILayout* pLyout);
		// 布局添加widget
		bool LayoutAddWidget(std::shared_ptr<IUIBase> widget);
		// 布局移除widget
		bool LayoutDelWidget(std::shared_ptr<IUIBase> widget);

	private:
		// SDL窗口指针
		SDL_Window* m_window = nullptr;
		// render
		std::shared_ptr<IRender> m_render = nullptr;
		// UI管理器
		std::shared_ptr<IUIManager> m_uiManager = nullptr;
		// 窗口宽高
		int m_width = 0;
		int m_height = 0;
	};
}
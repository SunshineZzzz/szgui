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
			const std::string& title, const uint32_t& width, const uint32_t& height);
		// Run循环
		void Run();

	private:
		// SDL窗口指针
		SDL_Window* m_window = nullptr;
		// render
		std::shared_ptr<IRender> m_render = nullptr;
		// UI管理器
		std::shared_ptr<IUIManager> m_uiManager = nullptr;
	};
}
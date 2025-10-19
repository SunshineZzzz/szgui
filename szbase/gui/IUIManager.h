#pragma once

#include <any>
#include <tuple>

#include "IUIBase.h"

namespace sz_gui 
{
	// 前置声明
	class IUIBase;

	// UI管理器抽象
	class IUIManager
	{
	public:
		IUIManager() = default;
		~IUIManager() = default;

		IUIManager(const IUIManager&) = delete;
		IUIManager& operator=(const IUIManager&) = delete;

		IUIManager(IUIManager&&) = delete;
		IUIManager& operator=(IUIManager&&) = delete;

	public:
		// 注册顶层UI
		virtual bool RegTopUI(std::shared_ptr<IUIBase>) = 0;
		// 注销顶层UI
		virtual bool UnRegTopUI(std::shared_ptr<IUIBase>) = 0;
		// 注册UI
		virtual bool RegUI(std::shared_ptr<IUIBase> ui) = 0;
		// 注销UI
		virtual bool UnRegUI(std::shared_ptr<IUIBase>) = 0;
		// 处理事件
		virtual bool HandleEvent(const std::any&) = 0;
	};
}
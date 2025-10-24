#pragma once

#include <any>
#include <tuple>
#include <memory>

#include "IUIBase.h"

namespace sz_gui 
{
	// 前置声明
	class IUIBase;
	class IRender;
	class ILayout;

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
		// 初始化
		virtual void Init(int, int) = 0;
		// 获取render
		virtual const std::shared_ptr<IRender>& GetRender() const = 0;
		// 注册顶层UI
		virtual bool RegTopUI(std::shared_ptr<IUIBase>) = 0;
		// 注销顶层UI
		virtual bool UnRegTopUI(std::shared_ptr<IUIBase>) = 0;
		// 注册UI
		virtual bool RegUI(std::shared_ptr<IUIBase>) = 0;
		// 注销UI
		virtual bool UnRegUI(std::shared_ptr<IUIBase>) = 0;
		// 处理事件
		virtual bool HandleEvent(std::any) = 0;
		// 设置布局
		virtual void SetLayout(ILayout*) = 0;
		// 布局添加widget
		virtual bool LayoutAddWidget(std::shared_ptr<IUIBase>) = 0;
		// 布局移除widget
		virtual bool LayoutDelWidget(std::shared_ptr<IUIBase>) = 0;
		// 绘制
		virtual void Render() = 0;
		// 添加脏UI
		virtual bool AddDirtyUI(std::shared_ptr<IUIBase>&) = 0;
		// 清除所有脏矩形UI
		virtual void ClearAllDirtyUI() = 0;
	};
}
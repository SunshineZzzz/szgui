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
		// 初始化
		virtual std::tuple<std::string, bool> Init() = 0;
	};
}
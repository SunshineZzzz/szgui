#pragma once

#include <cstdint>
#include <unordered_map>

namespace sz_gui 
{
	// 前置声明
	class IUIBase;

	// 矩形结构体
	struct Rect
	{
		float m_x, m_y;
		float m_width, m_height;

		Rect() : m_x(0), m_y(0), m_width(0), m_height(0) {}
		Rect(float x, float y, float width, float height) : m_x(x), m_y(y), m_width(width), m_height(height) {}
	};

	// 容忍度
	static constexpr float EPSILON = 0.00001f;
	// 比较仿函数
	struct UIKeyCompareAscending
	{
		bool operator()(const std::pair<uint64_t, float>& lhs,
			const std::pair<uint64_t, float>& rhs) const
		{
			// Z值排序
			float diff = lhs.second - rhs.second;

			// 检查是否在容忍范围内，视为相等
			if (std::abs(diff) > EPSILON)
			{
				// 不在容忍范围内，进行排序
				return lhs.second < rhs.second;
			}
			// Z值相同时(或在容忍范围内)，按照加入顺序排序
			return lhs.first < rhs.first;
		}
	};
	using ChildMultimap = std::multimap<std::pair<uint64_t, float>, 
		std::shared_ptr<IUIBase>, UIKeyCompareAscending>;
	using ChildUnorderedMap = std::unordered_map<uint64_t, ChildMultimap::iterator>;
	using TopChildMultimap = std::multimap<std::pair<uint64_t, float>, 
		std::shared_ptr<IUIBase>, UIKeyCompareAscending>;
	using TopChildUnorderedmap = std::unordered_map<uint64_t, TopChildMultimap::iterator>;
	using AllUIMultimap = std::multimap<std::pair<uint64_t, float>,
		std::shared_ptr<IUIBase>, UIKeyCompareAscending>;
	using AllChildUnorderedmap = std::unordered_map<uint64_t, AllUIMultimap::iterator>;
}
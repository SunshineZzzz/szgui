#include "UIFrame.h"

#include <glm/glm.hpp>
#include <memory>
#include <cassert>

namespace sz_gui
{
	namespace widget
	{
		UIFrame::UIFrame(std::string name, layout::AnchorPoint type, layout::Margins margins, 
			uint32_t desiredW, uint32_t desiredH, float desiredZ)
		{
			m_type = UIType::Frame;

			m_name = std::move(name);
			m_anchorPoint = type;
			m_margins = std::move(margins);
			m_desireWidth = (float)desiredW;
			m_desireHeight = (float)desiredH;
			m_z = desiredZ;
		}

		bool UIFrame::OnMouseButton(const events::MouseButtonEventData& data)
		{
			// m_eventbus.Publish<events::MouseButtonEvent>(data);
			return false;
		}

		void UIFrame::OnWindowRedraw()
		{
			m_dirty = true;

			// 重新布局
			auto rect = GetRect().SubtractBorder(GetBorderWidth());
			m_layout->SetParentRect(rect);
			m_layout->PerformLayout();

			OnCollectRenderData();
		}

		void UIFrame::OnCollectRenderData()
		{
			if (!m_dirty)
			{
				return;
			}

			auto aabb = getIntersectWithParent();
			if (aabb.IsNull())
			{
				return;
			}

			if (!m_parent.expired() && aabb.GetRect() != GetRect()) [[unlikely]]
			{
				// 布局引擎有bug
				assert(0);
			}

			// 生成渲染数据
			std::vector<float> positions = 
			{
				// 左上角
				m_x, m_y, m_z,
				// 右上角
				m_x + m_width, m_y, m_z,
				// 右下角
				m_x + m_width, m_y + m_height, m_z,
				// 左下角
				m_x, m_y + m_height, m_z,
			};
			// 顶点数据索引
			std::vector<uint32_t> indices = { 0, 1, 2, 3 };

			auto borderWidth = GetBorderWidth();
			// 绘制命令
			DrawCommand dCmd;
			dCmd.m_type = m_type;
			dCmd.m_drawMode = DrawMode::LINE_LOOP;
			dCmd.m_renderState = dCmd.m_renderState | RenderState::EnableScissorTest;
			dCmd.m_materialType = MaterialType::ColorMaterial;
			dCmd.m_scissorTest = { m_x + borderWidth, m_y + borderWidth,
				(m_width - 2 * borderWidth), (m_height - 2 * borderWidth) };
			dCmd.m_uiData = &m_uiData;

			auto& render = m_uiManager.lock()->GetRender();
			render->AppendDrawData(std::move(positions), std::move(indices), dCmd);

			// 递归收集子节点的渲染数据
			for (auto& child : m_childMultimap)
			{
				child.second->OnCollectRenderData();
			}
		}
	}
}
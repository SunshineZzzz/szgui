#include "UIFrame.h"

#include <glm/glm.hpp>

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
			m_width = (float)desiredW;
			m_height = (float)desiredH;
			m_z = desiredZ;
		}

		bool UIFrame::OnMouseButton(const events::MouseButtonEventData& data)
		{
			// m_eventbus.Publish<events::MouseButtonEvent>(data);
			return false;
		}

		void UIFrame::OnWindowSizeChange()
		{
			// 重新布局
			m_layout->SetParentRect(GetRect());
			m_layout->PerformLayout();

			// 重新收集渲染数据
			OnCollectRenderData();
			return;
		}

		void UIFrame::OnCollectRenderData()
		{
			glm::vec3 pos = GetPos();
			sz_ds::AABB2D aabb(m_x, m_y, m_x + m_width, m_y + m_height);

			if (!HasUIFlag(UIFlag::Top))
			{
				pos += m_parent.lock()->GetPos();

				aabb = getIntersectWithParent();
				if (!aabb.IsNull())
				{
					return;
				}
			}

			// 生成渲染数据
			std::vector<sz_ds::Vertex> vertexVec =
			{
				{
					// 左上角
					{m_x, m_y, m_z},
					// 贴图区域的左上角
					{m_uvLT.x, m_uvLT.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
				{
					// 右上角
					{m_x + m_width, m_y, m_z},
					// 贴图区域的右上角
					{m_uvRB.x, m_uvLT.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
				{
					// 右下角
					{m_x + m_width, m_y + m_height, m_z},
					// 贴图区域的右下角
					{m_uvRB.x, m_uvRB.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
				{
					// 左下角
					{m_x, m_y + m_height, m_z},
					// 贴图区域的左下角
					{m_uvLT.x, m_uvRB.y, 0.0f},
					{m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a},
				},
			};

			// 顶点数据索引
			std::vector<uint32_t> indicesVec = { 0, 1, 3, 4 };

			// 绘制命令
			DrawCommand dCmd
			{
				m_type,
				DrawMode::LINE_LOOP,
				RenderState::EnableScissorTest | RenderState::EnableDepthTest,
				indicesVec.size(),
				{m_x+1, m_y+1, m_width-2, m_height-2},
				m_texture2dUintId,
				m_shaderId,
			};

			m_uiManager.lock()->AppendDrawData(vertexVec, indicesVec, dCmd);
		}
	}
}
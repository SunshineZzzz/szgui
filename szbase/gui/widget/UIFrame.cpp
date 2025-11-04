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
			m_z = -desiredZ;
		}

		bool UIFrame::OnMouseButton(const events::MouseButtonEventData& data)
		{
			// m_eventbus.Publish<events::MouseButtonEvent>(data);
			return false;
		}

		void UIFrame::OnWindowResize()
		{
			UIBase::OnWindowResize();

			// 重新布局
			auto rect = GetRect().SubtractBorder(m_borderWidth);
			m_layout->SetParentRect(rect);
			m_layout->PerformLayout();
		}

		void UIFrame::OnCollectRenderData()
		{
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

			// 顶点位置信息索引
			std::vector<float> positions =
			{
				// 左上角
				0.0f, 0.0f, 0.0f,
				// 右上角
				m_width, 0.0f, 0.0f,
				// 右下角
				m_width, m_height, 0.0f,
				// 左下角
				0.0f, m_height, 0.0f,
			};
			// 顶点颜色信息
			std::vector<float> colors = 
			{
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
			};
			// 顶点数据索引
			std::vector<uint32_t> indices = { 0, 1, 2, 3 };

			// 绘制命令
			DrawCommand dCmd;
			dCmd.m_onlyId = m_childIdForUIManager;
			dCmd.m_worldPos = { m_x, m_y, m_z };
			dCmd.m_uploadOp = getUploadOp();
			dCmd.m_drawMode = DrawMode::LINE_LOOP;
			dCmd.m_materialType = MaterialType::ColorMaterial;

			auto& render = m_uiManager.lock()->GetRender();
			render->AppendDrawData(positions, colors, indices, dCmd);

			// 递归收集子节点的渲染数据
			//for (auto& child : m_childMultimap)
			//{
			//	child.second->OnCollectRenderData();
			//}
		}
	}
}
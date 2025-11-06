#include "UIFrame.h"

#include <glm/glm.hpp>

#include <memory>
#include <cassert>

namespace sz_gui
{
	namespace widget
	{
		UIFrame::UIFrame(std::string name, layout::AnchorPoint type, layout::Margins margins, 
			uint32_t desiredW, uint32_t desiredH)
		{
			m_type = UIType::Frame;

			m_name = std::move(name);
			m_anchorPoint = type;
			m_margins = std::move(margins);
			m_desireWidth = (float)desiredW;
			m_desireHeight = (float)desiredH;

			SetColorTheme(ColorTheme::LightMode);
		}

		bool UIFrame::OnMouseLeftButtonClick()
		{
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

		bool UIFrame::OnCollectRenderData()
		{
			if (!IsVisible())
			{
				return false;
			}

			auto aabb = getIntersectWithParent();
			if (aabb.IsNull())
			{
				return false;
			}

			if (!m_parent.expired() && aabb.GetRect() != GetRect()) [[unlikely]]
			{
				// 布局引擎有bug
				assert(0);
			}

			// 顶点位置信息索引
			static std::vector<float> positions;
			positions.clear();
			positions =
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
			const std::vector<float>& colors = m_colors;
			// 顶点数据索引
			static std::vector<uint32_t> indices = { 0, 1, 2, 3 };

			// 绘制命令
			DrawCommand dCmd;
			dCmd.m_onlyId = m_childIdForUIManager;
			dCmd.m_worldPos = { m_x, m_y, m_z };
			dCmd.m_uploadOp = getUploadOp();
			dCmd.m_drawMode = DrawMode::LINE_LOOP;
			dCmd.m_renderState |= RenderState::EnableScissorSet;
			dCmd.m_scissorTest = {true, int32_t(m_x + m_borderWidth), int32_t(m_y + m_borderWidth),
				int32_t(m_width - 2 * m_borderWidth), int32_t(m_height - 2 * m_borderWidth) };

			auto& render = m_uiManager.lock()->GetRender();
			render->AppendDrawData(positions, colors, indices, dCmd);

			static bool isChildAppend = false;
			// 递归收集子节点的渲染数据
			for (auto& child : m_childMultimap)
			{
				if (child.second->OnCollectRenderData())
				{
					isChildAppend = true;
				}
			}

			DrawCommand dExtraCmd;
			dExtraCmd.m_onlyId = m_childIdForUIManager;
			dExtraCmd.m_renderState = RenderState::None;
			if (!m_childMultimap.empty() || isChildAppend)
			{
				dExtraCmd.m_renderState = RenderState::EnableScissorSet;
				dExtraCmd.m_scissorTest = { false };
				dExtraCmd.m_onlyId = 
					std::prev(m_childMultimap.end())->second->GetChildIdForUIManager();
			}
			render->ExtraAppendDrawCommand(dExtraCmd);

			return true;
		}

		void UIFrame::SetColorTheme(ColorTheme theme)
		{
			UIBase::SetColorTheme(theme);

			switch (theme)
			{
			case ColorTheme::LightMode:
			m_colors =
			{
				0.85f, 0.85f, 0.85f,
				0.85f, 0.85f, 0.85f,
				0.85f, 0.85f, 0.85f,
				0.85f, 0.85f, 0.85f,
			};
			break;
			}
		}
	}
}
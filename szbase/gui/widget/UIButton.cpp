#include "UIButton.h"
#include "../InputControl.h"
#include "../../string/String.h"

namespace sz_gui
{
	namespace widget
	{
		UIButton::UIButton(std::string name, layout::AnchorPoint type, layout::Margins margins, 
			uint32_t desiredW, uint32_t desiredH)
		{
			m_type = UIType::Button;

			m_name = std::move(name);
			m_anchorPoint = type;
			m_margins = std::move(margins);
			m_desireWidth = (float)desiredW;
			m_desireHeight = (float)desiredH;

			SetColorTheme(ColorTheme::LightMode);
            SetUIFlag(UIFlag::Interactive);
		}

        bool UIButton::OnMouseLeftButtonClick()
        {
            m_mouseLeftIsDown = false;
            setState(ButtonState::Normal);
            return false;
        }

        void UIButton::OnMouseLeftButtonDown()
        {
            m_mouseLeftIsDown = true;
            setState(ButtonState::Press);
        }

        void UIButton::OnMouseLeftButtonUp()
        {
            m_mouseLeftIsDown = false;
            setState(ButtonState::Normal);
        }

        void UIButton::OnMouseMoveEnter()
        {
            if (m_mouseLeftIsDown)
            {
                return;
            }
            setState(ButtonState::Hover);
        }

        void UIButton::OnMouseMove()
        {
            if (m_mouseLeftIsDown)
            {
                return;
            }
            setState(ButtonState::Hover);
        }

        void UIButton::OnMouseMoveLeave()
        {
            if (m_mouseLeftIsDown)
            {
                return;
            }
            setState(ButtonState::Normal);
        }

		bool UIButton::OnCollectRenderData()
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
			const std::vector<float>& colors = m_colors[m_state];
			// 顶点数据索引
			static std::vector<uint32_t> indices = 
			{
				0, 1, 2, 
				2, 3 ,0, 
			};
            // 上传操作
            auto uploadOp = getUploadOp();
			// 绘制命令
			DrawCommand dCmd;
			dCmd.m_onlyId = m_childIdForUIManager;
			dCmd.m_worldPos = { m_x, m_y, m_z };
			dCmd.m_uploadOp = uploadOp;
			dCmd.m_drawMode = DrawMode::TRIANGLES;

			auto& render = m_uiManager.lock()->GetRender();
			render->AppendDrawData(positions, colors, indices, dCmd);
            // 加入绘制文字数据
            appendTextDrawData(uploadOp);

			return true;
		}

        void UIButton::SetColorTheme(ColorTheme theme)
        {
            UIBase::SetColorTheme(theme);

            // 默认蓝 (Normal)
            const float R_NORM = 0.25f; const float G_NORM = 0.60f; const float B_NORM = 0.90f;
            // 亮蓝 (悬停 - Hover): 明显变亮
            const float R_HOV = 0.35f; const float G_HOV = 0.70f; const float B_HOV = 0.98f;
            // 暗蓝 (按下 - Press): 明显变暗
            const float R_PRS = 0.15f; const float G_PRS = 0.45f; const float B_PRS = 0.75f;
            // 褪色蓝 (禁用 - Disable): 褪色且发灰
            const float R_DIS = 0.70f; const float G_DIS = 0.80f; const float B_DIS = 0.88f;

            switch (theme)
            {
            case ColorTheme::LightMode:
                m_colors[ButtonState::Disable] =
                {
                    R_DIS, G_DIS, B_DIS, R_DIS, G_DIS, B_DIS,
                    R_DIS, G_DIS, B_DIS, R_DIS, G_DIS, B_DIS,
                };
                break;
            }

            switch (theme)
            {
            case ColorTheme::LightMode:
                m_colors[ButtonState::Normal] =
                {
                    R_NORM, G_NORM, B_NORM, R_NORM, G_NORM, B_NORM,
                    R_NORM, G_NORM, B_NORM, R_NORM, G_NORM, B_NORM,
                };
                break;
            }

            switch (theme)
            {
            case ColorTheme::LightMode:
                m_colors[ButtonState::Hover] =
                {
                    R_HOV, G_HOV, B_HOV, R_HOV, G_HOV, B_HOV,
                    R_HOV, G_HOV, B_HOV, R_HOV, G_HOV, B_HOV,
                };
                break;
            }

            // press
            switch (theme)
            {
            case ColorTheme::LightMode:
                m_colors[ButtonState::Press] =
                {
                    R_PRS, G_PRS, B_PRS, R_PRS, G_PRS, B_PRS,
                    R_PRS, G_PRS, B_PRS, R_PRS, G_PRS, B_PRS,
                };
                break;
            }
        }

        void UIButton::setState(ButtonState state)
        {
            if (m_state == state)
            {
                return;
            }

            m_state = state;
            setUploadOp(UploadOperation::UploadColorOrUv);
        }

        void UIButton::appendTextDrawData(UploadOperation uploadOp)
        {
            if (sz_string::IsOnlyWhitespace(m_text))
            {
                return;
            }

            // 绘制命令
            DrawCommand dCmd;
            dCmd.m_drawTarget = DrawTarget::Text;
            dCmd.m_onlyId = m_childIdForUIManager;
            dCmd.m_worldPos = { m_x, m_y, m_z };
            dCmd.m_uploadOp = uploadOp;
            dCmd.m_drawMode = DrawMode::TRIANGLES;
            dCmd.m_renderState = dCmd.m_renderState | RenderState::EnableBlend;
            dCmd.m_materialType = MaterialType::TextMaterial;

        }
	}
}
#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <cstdint>

#include "../utils/BitwiseEnum.h"
#include "IUIBase.h"

namespace sz_gui
{
	// 面剔除
	struct FaceCulling
	{
		// 顶点环绕顺序
		enum class FrontFaceType
		{
			// 逆时针
			CCW,
			// 顺时针
			CW,
		};
		// 剔除方式
		enum class CullFaceType
		{
			// 剔除正面
			Front,
			// 剔除背面
			Back,
		};
		// 正面
		FrontFaceType m_frontFace = FrontFaceType::CCW;
		// 剔除正面还是背面
		CullFaceType m_cullFace = CullFaceType::Back;
	};

	// 剪裁测试
	struct ScissorTest
	{
		float m_x, m_y;
		float m_width, m_height;
	};

	// 深度测试
	struct DepthTest
	{
		// 深度测试函数
		enum class DepthFuncType
		{
			// 小于
			Less,
			// 小于等于
			LessEqual,
			// 等于
			Equal,
			// 大于
			Greater,
			// 大于等于
			GreaterEqual,
			// 不等于
			NotEqual,
			// 总是
			Always,
		};
		// 深度测试函数
		DepthFuncType m_depthFunc = DepthFuncType::Less;
		// 是否写入深度值
		bool m_depthWrite{ true };
	};

	// 混合
	struct Blend
	{
		// 混合函数
		enum class BlendFuncType
		{
			// C_source的alpha值
			SRC_ALPHA,
			// 1.0f - C_source的alpha值
			ONE_MINUS_SRC_ALPHA,
		};
		// C_result = C_source * F_source + C_destination * F_destination
		// 源颜色混合函数，F_source
		BlendFuncType m_srcBlendFunc = BlendFuncType::SRC_ALPHA;
		// 目标颜色混合函数，F_destination
		BlendFuncType m_dstBlendFunc = BlendFuncType::ONE_MINUS_SRC_ALPHA;
		// 0.0f~1.0f，0.0f完全透明，1.0f完全不透明
		// 作用于物体本身透明度
		float m_opacity{ 1.0f };
	};

	// 绘制状态
	enum class RenderState : uint32_t
	{
		// 默认
		None = 1 << 0,
		// 开启面剔除
		EnableFaceCulling = 1 << 1,
		// 开启剪裁测试
		EnableScissorTest = 1 << 2,
		// 开启深度测试
		EnableDepthTest = 1 << 3,
		// 开启混合
		EnableBlend = 1 << 4,
	};

	USING_BITMASK_OPERATORS()
}
ENABLE_BITMASK_OPERATORS(sz_gui::RenderState)

namespace sz_gui
{
	// 绘制模式
	enum class DrawMode
	{
		// 默认
		None,
		// 连续绘制线段，并自动连接首尾
		LINE_LOOP,
	};

	// 材质类型
	enum class MaterialType
	{
		// 颜色
		ColorMaterial,
		// 贴图
		TextureMaterial,
		// 文字
		TextMaterial,
	};

	// 绘制命令，描述一次绘制调用
	struct DrawCommand 
	{
		// UI类型
		UIType m_type = UIType::None;
		// 绘制模式
		DrawMode m_drawMode = DrawMode::None;
		// 绘制状态
		RenderState m_renderState = RenderState::EnableFaceCulling | 
			RenderState::EnableDepthTest | RenderState::EnableBlend;
		// 材质类型
		MaterialType m_materialType = MaterialType::ColorMaterial;
		// 面剔除参数
		FaceCulling m_faceCulling;
		// 剪裁测试参数
		ScissorTest m_scissorTest;
		// 深度测试参数
		DepthTest m_depthTest;
		// 混合参数
		Blend m_blend;
		// 顶点数据索引数量
		size_t m_indexCount = 0;
		// UI数据
		std::any m_uiData = nullptr;
	};

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
		// 加入绘制数据
		virtual void AppendDrawData(std::vector<float>&& positions, 
			std::vector<uint32_t>&& indices, DrawCommand cmdTemplate) = 0;
		// 全量绘制
		virtual void FullDraw() = 0;
		// 增量绘制
		virtual void IncDraw() = 0;
		// 获取线宽范围
		virtual std::tuple<float, float> GetLineWidthRange() const = 0;
	};
}
#pragma once

#include <string>
#include <tuple>
#include <vector>
#include <cstdint>

#include "../ds/Math.h"
#include "../utils/BitwiseEnum.h"
#include "IUIBase.h"

namespace sz_gui
{
	// 绘制状态
	enum class RenderState : uint32_t
	{
		// 默认
		None = 1 << 0,
		// 开启剪裁测试
		EnableScissorTest = 1 << 1,
		// 开启深度测试
		EnableDepthTest = 1 << 2,
		// 开启混合
		EnableBlend = 1 << 3,
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

	// 绘制命令，描述一次绘制调用
	struct DrawCommand 
	{
		// UI类型
		UIType m_type = UIType::None;
		// 绘制模式
		DrawMode m_drawMode = DrawMode::None;
		// 绘制状态
		RenderState m_renderState = RenderState::None;
		// 顶点数据索引数量
		uint32_t m_indexCount = 0;
		// 要使用的纹理ID
		uint32_t m_textureId = 0;
		// 着色器Id
		uint32_t m_shaderId = 0;
		// 线宽
		float m_lineWidth = 1.0f;
		// 顶点数据偏移
		uint32_t m_vertexOffset = 0;
		// 顶点数据索引偏移
		uint32_t m_indexOffset = 0;
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
		// 准备Shader
		virtual std::tuple<std::string, bool> PrepareShader(const char* vertexShaderSource,
			const char* fragmentShaderSource, const char* name = "default")
		{
			return PrepareShaderImpl(vertexShaderSource, fragmentShaderSource, name);
		}
		// 根据名称获取ShaderId
		virtual uint32_t GetShaderIdByName(const std::string& name = "default") const
		{
			return GetShaderIdByNameImpl(name);
		}

	public:
		// 初始化
		virtual std::tuple<std::string, bool> Init() = 0;
		// 准备Shader
		virtual std::tuple<std::string, bool> PrepareShaderImpl(const char*, const char*, const char*) = 0;
		// 准备2D纹理
		virtual std::tuple<std::string, bool> PrepareTexture2D(const std::vector<std::string>&, 
			const std::vector<uint32_t>&) = 0;
		// 根据名称获取ShaderId
		virtual uint32_t GetShaderIdByNameImpl(const std::string&) const = 0;
		// 加入绘制数据
		virtual void AppendDrawData(const std::vector<sz_ds::Vertex>& vertices, 
			const std::vector<uint32_t>& indices, DrawCommand cmdTemplate) = 0;
		// 压入剪裁区域
		virtual void PushScissor(const sz_ds::Rect& rect) = 0;
		// 弹出剪裁区域
		virtual void PopScissor() = 0;
		// 全量绘制
		virtual void FullDraw() = 0;
		// 增量绘制
		virtual void IncDraw() = 0;
	};
}
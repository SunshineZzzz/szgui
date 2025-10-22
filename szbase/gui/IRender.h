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
		// 开启混合
		EnableBlend = 1 << 0,
		// 开启剪裁测试
		EnableScissorTest = 1 << 1,
	};

	USING_BITMASK_OPERATORS()
}
ENABLE_BITMASK_OPERATORS(sz_gui::RenderState)

namespace sz_gui
{
	// 绘制模式
	enum class DrawMode
	{
		// 连续绘制线段，并自动连接首尾
		LINE_LOOP,
	};

	// 绘制命令，描述一次绘制调用
	struct DrawCommand 
	{
		// UI类型
		UIType m_type;
		// 绘制模式
		DrawMode m_drawMode;
		// 绘制状态
		RenderState m_renderState;
		// 裁剪区域
		Rect scissor;
		// 要使用的纹理ID
		uint32_t m_textureId;
		// 着色器Id
		uint32_t m_shaderId;
		// 顶点数据偏移
		uint32_t m_vertexOffset;
		// 顶点数据索引偏移
		uint32_t m_indexOffset;
		// 顶点数据索引数量
		uint32_t m_indexCount;
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
		// 带默认值设置清除颜色
		virtual bool SetClearColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) 
		{
			return SetClearColorImpl(r, g, b, a);
		}
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
		// 设置视口
		virtual bool SetViewPort(int,int,int,int) = 0;
		// 设置清除颜色
		virtual bool SetClearColorImpl(float, float, float, float) = 0;
		// 清除屏幕
		virtual bool Clear() = 0;
		// 交换缓冲区
		virtual bool SwapWindow() = 0;
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
	};
}
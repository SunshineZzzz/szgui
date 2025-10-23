// comment: OpenGLES渲染器

#pragma once

#include <SDL3/SDL.h>
#include <GLES2/gl2.h>

#include <tuple>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>

#include "../IRender.h"
#include "Texture2D.h"
#include "Shader.h"
#include "../../ds/Math.h"

namespace sz_gui 
{
    namespace gles
    {
        class GLESContext : public IRender
        {
        public:
            // GL属性设置
            static std::tuple<std::string, bool> InitGLESAttributes();

        public:
            GLESContext(SDL_Window* window);
            ~GLESContext();

            // 生成shaderId
            const uint32_t GenShaderId()
            {
                if (m_nextShaderId == 0) [[unlikely]]
                {
                    m_nextShaderId = 1;
                }
                auto id = m_nextShaderId++;
                if (m_shaderMap.find(id) != m_shaderMap.end()) [[unlikely]]
                {
                    assert(0);
                }
                return id;
            }

            // 初始化
            std::tuple<std::string, bool> Init() override;
            // 设置视口
            bool SetViewPort(int, int, int, int) override;
            // 设置清除颜色
            bool SetClearColorImpl(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f) override;
            // 清除屏幕
            bool Clear() override;
            // 交换缓冲区
            bool SwapWindow() override;
            // 准备Shader
            std::tuple<std::string, bool> PrepareShaderImpl(const char* vertexShaderSource,
                const char* fragmentShaderSource, const char* name) override;
            // 准备2D纹理
            std::tuple<std::string, bool> PrepareTexture2D(const std::vector<std::string>& vPaths, 
                const std::vector<uint32_t>& vUnits) override;
            // 根据名称获取ShaderId
            uint32_t GetShaderIdByNameImpl(const std::string& name) const override
            {
                auto it = m_shaderUnorderdmap.find(name);
				if (it != m_shaderUnorderdmap.end())
				{
					return it->second->first;
				}
				return 0;
            }
            // 加入绘制数据
            void AppendDrawData(const std::vector<sz_ds::Vertex>& vertices,
                const std::vector<uint32_t>& indices, DrawCommand cmdTemplate) override;
            // 设置剪裁区域
            void Scissor(int x, int y, int width, int height)
            {
                glScissor(x, y, width, height);
            }
            // 开启渲染状态
            void EnableRenderState(RenderState state, std::any data = {});
            // 关闭渲染状态
            void DisableRenderState(RenderState state);
            // 开始使用Shader
            void BeginUseShader(uint32_t shaderId);
            // 结束使用Shader
            void EndUserShader();
            // 绑定2D纹理
            void BeginUseTexture2D(uint32_t texture2dId);


        private:
            // SDL窗口指针
            SDL_Window* m_window = nullptr;
            // gles上下文
            SDL_GLContext m_glesContext = nullptr;
            // 生成的着色器Id
            uint32_t m_nextShaderId = 1;
            // 着色器Id<->着色器对象
            std::map<uint32_t, std::unique_ptr<Shader>> m_shaderMap;
            // 着色器名称<->着色器对象迭代器
            std::unordered_map<std::string, std::map<uint32_t, std::unique_ptr<Shader>>::iterator> 
                m_shaderUnorderdmap;
            // 纹理单元Id<->2D纹理对象
            std::map<uint32_t, std::unique_ptr<Texture2D>> m_texture2dMap;
            // 绘制命令集合
            std::vector<DrawCommand> m_drawCommandsVec;
            // 绘制顶点集合
            std::vector<sz_ds::Vertex> m_vertexVec;
            // 绘制索引集合
            std::vector<uint32_t> m_indicesVec;
            // 当前渲染状态
            RenderState m_curRenderState = RenderState::None;
            // 当前使用的ShaderId
            uint32_t m_curShaderId = 0;
            // 当前使用的2d纹理单元Id
            uint32_t m_curTexture2dId = 0;
        };
    }
}
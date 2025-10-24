// comment: OpenGLES渲染器

#pragma once

#include <SDL3/SDL.h>
#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tuple>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stack>

#include "../IRender.h"
#include "Texture2D.h"
#include "Shader.h"
#include "../../ds/Math.h"
#include "OrthographicCamera.h"

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
            // 压入剪裁区域
            void PushScissor(const sz_ds::Rect& rect) override;
            // 弹出剪裁区域
            void PopScissor() override;
            // 全量绘制
            void FullDraw() override;
            // 增量绘制
            void IncDraw() override;

        public:
            // 设置视口
            void SetViewPort(int, int, int, int);
            // 设置清除颜色
            void SetClearColor(float red = 1.0f, float green = 1.0f, float blue = 1.0f, float alpha = 1.0f);
            // 清除屏幕
            void Clear();
            // 交换缓冲区
            bool SwapWindow();
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
            // 获取窗口大小
            std::pair<const int, const int> GetWindowSize() const 
            {
                int width = 0;
                int height = 0;
                SDL_GetWindowSize(m_window, &width, &height);
                return std::make_pair(width, height);
            }
            // 获取当前视口和裁剪区域交集
            sz_ds::AABB2D GetViewportAndScissorRectIntersection(const sz_ds::Rect& rect) const
            {
				auto [width, height] = GetWindowSize();
				sz_ds::AABB2D viewportRect = sz_ds::AABB2D(0.0f, 0.0f, (float)width, (float)height);
                return viewportRect.Intersection(rect.ToAABB2D());
            }
            // 设置线宽
            void SetLineWidth(float width)
            {
                if (width != m_curLineWidth)
                {
                    glLineWidth(width);
                    m_curLineWidth = width;
                }
            }
            // 准备摄像机
            void PrepareCamera(int width, int height)
            {
                m_camera = std::make_unique<OrthographicCamera>(0.0f, float(width), float(height), 0.0f, 1000.0f, -1000.0f);
                m_viewMatrix = m_camera->GetViewMatrix();
                m_projectionMatrix = m_camera->GetProjectionMatrix();
            }

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
            // 当前线宽
            float m_curLineWidth = 1.0f;
            // 当前裁剪区域
            sz_ds::Rect m_curScissorRect = sz_ds::Rect();
            // 用于保存嵌套的裁剪区域
            std::stack<sz_ds::Rect> m_scissorStack;
            // 模型变化矩阵
            glm::mat4 m_modelMatrix = glm::identity<glm::mat4>();
            // 摄像机，视图变化矩阵
            glm::mat4 m_viewMatrix = glm::identity<glm::mat4>();
            // 投影变化矩阵
            glm::mat4 m_projectionMatrix = glm::identity<glm::mat4>();
            // 摄像机
            std::unique_ptr<Camera> m_camera = nullptr;
        };
    }
}
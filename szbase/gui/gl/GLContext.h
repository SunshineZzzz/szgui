// comment: OpenGL上下文

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include <tuple>
#include <string>
#include <vector>
#include <memory>

#include "../IRender.h"
#include "Shader.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "RenderItem.h"
#include "CheckRstErr.h"

namespace sz_gui 
{
    namespace gl
    {
        class GLContext : public IRender
        {
        public:
            // GL属性设置
            static std::tuple<std::string, bool> InitGLAttributes();

        public:
            GLContext(SDL_Window* window);
            ~GLContext();

            // 初始化
            std::tuple<std::string, bool> Init(int width, int height) override;
            // 加入绘制数据
            void AppendDrawData(const glm::vec3& pos, const std::vector<float>& positions,
                const std::vector<float>& colorOrUVs, const std::vector<uint32_t>& indices,
                DrawCommand cmd) override;
            // 渲染
            void Render() override;
            // 窗口大小改变事件
            void OnWindowResize(int width, int height) override
            {
                GL_CALL(glViewport(0, 0, width, height));
                prepareCamera(width, height);
            }

        private:
            // 获取绘制命令
            GLenum getDrawMode(DrawMode mode);
            // 绘制对象
            void renderObject(const std::unique_ptr<RenderItem>& ri);
            // 根据Material类型不同，挑选不同的shader
            std::unique_ptr<Shader>& pickShader(MaterialType type);
            // 混合相关，获取混合因子
            GLenum getBlendSFactor(BlendFuncType type);
            GLenum getBlendDFactor(BlendFuncType type);
            // 深度测试相关，获取深度测试函数
            GLenum getDepthFunc(DepthFuncType type);
            //  设置面剔除状态
            void setFaceCullingState(const std::unique_ptr<RenderItem>& ri);
            // 设置深度测试状态
            void setDepthState(const std::unique_ptr<RenderItem>& ri);
            // 设置混合状态
            void setBlendState(const std::unique_ptr<RenderItem>& ri);
            // 准备摄像机
            void prepareCamera(int width, int height)
            {
                m_camera = std::make_unique<OrthographicCamera>(0.0f, float(width), float(height), 
                    0.0f, 0.0f, 1000.f);
            }

        private:
            // SDL窗口指针
            SDL_Window* m_window = nullptr;
            // gl上下文
            SDL_GLContext m_glContext = nullptr;
            // 摄像机
            std::unique_ptr<Camera> m_camera = nullptr;
            // err shader
            std::unique_ptr<Shader> m_errShader{ nullptr };
            // 颜色shader
            std::unique_ptr<Shader> m_colorShader{ nullptr };
            // 材质shader
            std::unique_ptr<Shader> m_textureShader{ nullptr };
            // 文字shader
            std::unique_ptr<Shader> m_textShader{ nullptr };
            // 不透明绘制对象
            std::vector<std::unique_ptr<RenderItem>> m_opacityObjects;
            // 透明绘制对象
            std::vector<std::unique_ptr<RenderItem>> m_transparentObjects;
        };
    }
}
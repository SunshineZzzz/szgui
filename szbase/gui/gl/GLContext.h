// comment: OpenGL上下文

#pragma once

#ifdef USE_OPENGL_ES
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <SDL3/SDL.h>

#include <tuple>
#include <string>
#include <vector>
#include <memory>

#include "../IRender.h"

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
            std::tuple<std::string, bool> Init() override;
            // 加入绘制数据
            void AppendDrawData(std::vector<float>&& positions,
                std::vector<uint32_t>&& indices, DrawCommand cmdTemplate) override;
            // 全量绘制
            void FullDraw() override;
            // 增量绘制
            void IncDraw() override;
            // 获取线宽范围
            std::tuple<float, float> GetLineWidthRange() const;

        private:
            // SDL窗口指针
            SDL_Window* m_window = nullptr;
            // gl上下文
            SDL_GLContext m_glContext = nullptr;
            // 线宽范围
            GLfloat m_lineWidthRange[2] = { 1.0f };
        };
    }
}
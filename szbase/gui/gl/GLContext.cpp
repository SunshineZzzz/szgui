#include "GLContext.h"

#include <unordered_set>
#include <format>

namespace sz_gui
{
    namespace gl
    {
        std::tuple<std::string, bool> GLContext::InitGLAttributes()
        {
            std::string errMsg = "success";

            #ifdef USE_OPENGL_ES
            if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0))
            {
                errMsg = "gles version error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }
            #else
            if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6) ||
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE))
            {
                errMsg = "gl version error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }
            #endif

            // 一些基础属性
            if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) ||
                !SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) ||
                !SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) ||
                !SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8))
            {
                errMsg = "gl set base attribute error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            // 开启多重采样缓冲区 & 设置每像素的采样点数量
            if (!SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)||
                !SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4))
            {
                errMsg = "gl set multisample error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            return { errMsg, true };
        }

        GLContext::GLContext(SDL_Window* window)
        {
            m_window = window;
        }

        GLContext::~GLContext()
        {
            m_window = nullptr;

            if (m_glContext)
            {
                SDL_GL_DestroyContext(m_glContext);
                m_glContext = nullptr;
            }
        }

        std::tuple<std::string, bool> GLContext::Init()
        {
            std::string errMsg = "success";

            m_glContext = SDL_GL_CreateContext(m_window);
            if (!m_glContext)
            {
                errMsg = "sdl create gl context error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            if (!SDL_GL_MakeCurrent(m_window, m_glContext))
            {
                errMsg = "sdl make current error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            return { std::move(errMsg), true };
        }

        void GLContext::AppendDrawData(std::vector<float>&& positions,
             std::vector<uint32_t>&& indices, DrawCommand cmd)
        {
        }

        void GLContext::FullDraw()
        {
        }

        void GLContext::IncDraw()
        {
        }

        std::tuple<float, float> GLContext::GetLineWidthRange() const
        {
            return { m_lineWidthRange[0], m_lineWidthRange[1] };
        }
    }
}
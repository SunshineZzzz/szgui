#include "GLESContext.h"

namespace sz_gui
{
	std::tuple<std::string, bool> GLESContext::InitGLESAttributes()
	{
        std::string errMsg = "success";

        if (!SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES) ||
            !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2) ||
            !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0))
        {
            errMsg = "gles version error," + std::string(SDL_GetError());
            return { std::move(errMsg), false };
        }

        if (!SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) ||
            !SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8) ||
            !SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8) ||
            !SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) ||
            !SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8) ||
            !SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24))
        {
            errMsg = "gles set attribute error," + std::string(SDL_GetError());
            return { errMsg, false };
        }

        return { errMsg, true };
	}

    GLESContext::GLESContext(SDL_Window* window)
    {
        m_window = window;
    }
    
    GLESContext::~GLESContext()
    {
        m_window = nullptr;

        if (m_glcontext)
        {
            SDL_GL_DestroyContext(m_glcontext);
            m_glcontext = nullptr;
        }
    }

    std::tuple<std::string, bool> GLESContext::Init()
    {
        std::string errMsg = "success";

        m_glcontext = SDL_GL_CreateContext(m_window);
        if (!m_glcontext)
        {
            errMsg = "sdl create gles context error," + std::string(SDL_GetError());
            return { std::move(errMsg), false };
        }

        if (!SDL_GL_MakeCurrent(m_window, m_glcontext))
        {
            errMsg = "sdl make current error," + std::string(SDL_GetError());
            return { std::move(errMsg), false };
        }

        return { std::move(errMsg), true };
    }

    bool GLESContext::SetViewPort(int x, int y, int width, int height)
    {
        if (!m_window || !m_glcontext)
        {
            return false;
        }
        glViewport(x, y, width, height);
        return true;
    }

    bool GLESContext::SetClearColorImpl(float red, float green, float blue, float alpha)
    {
        if (!m_window || !m_glcontext)
        {
            return false;
        }
        glClearColor(red, green, blue, alpha);
        return true;
    }
    
    bool GLESContext::Clear()
    {
        if (!m_window || !m_glcontext)
        {
            return false;
        }
        // 清除颜色缓冲区(被清理为glClearColor设置的颜色)|清理深度缓冲区(被清理为1.0)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return true;
    }

    bool GLESContext::SwapWindow()
    {
        if (!m_window || !m_glcontext)
		{
			return false;
		}
        return SDL_GL_SwapWindow(m_window);
    }
}
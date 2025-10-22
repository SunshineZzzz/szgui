#include "GLESContext.h"

#include <unordered_set>
#include <format>

namespace sz_gui
{
    namespace gles
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

            if (m_glesContext)
            {
                SDL_GL_DestroyContext(m_glesContext);
                m_glesContext = nullptr;
            }
        }

        std::tuple<std::string, bool> GLESContext::Init()
        {
            std::string errMsg = "success";

            m_glesContext = SDL_GL_CreateContext(m_window);
            if (!m_glesContext)
            {
                errMsg = "sdl create gles context error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            if (!SDL_GL_MakeCurrent(m_window, m_glesContext))
            {
                errMsg = "sdl make current error," + std::string(SDL_GetError());
                return { std::move(errMsg), false };
            }

            // 开启深度检测
            glEnable(GL_DEPTH_TEST);
            // 深度检测的比较函数, GL_LESS: 当前片元深度值小于当前深度缓冲去中深度值时通过测试
            glDepthFunc(GL_LESS);

            // 开启混合
            glEnable(GL_BLEND);
            // 计算源像素(Source，新绘制的)和目标像素(Destination，颜色缓冲区中已有的的颜色混合方式，
            // Final_Color = Source_Color x Source_Alpha + Destination_Color x (1 - Source_Alpha)
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            return { std::move(errMsg), true };
        }

        bool GLESContext::SetViewPort(int x, int y, int width, int height)
        {
            if (!m_window || !m_glesContext)
            {
                return false;
            }
            glViewport(x, y, width, height);
            return true;
        }

        bool GLESContext::SetClearColorImpl(float red, float green, float blue, float alpha)
        {
            if (!m_window || !m_glesContext)
            {
                return false;
            }
            glClearColor(red, green, blue, alpha);
            return true;
        }

        bool GLESContext::Clear()
        {
            if (!m_window || !m_glesContext)
            {
                return false;
            }
            // 清除颜色缓冲区(被清理为glClearColor设置的颜色)|清理深度缓冲区(被清理为1.0)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            return true;
        }

        bool GLESContext::SwapWindow()
        {
            if (!m_window || !m_glesContext)
            {
                return false;
            }
            return SDL_GL_SwapWindow(m_window);
        }

        std::tuple<std::string, bool> GLESContext::PrepareTexture2D(const std::vector<std::string>& vPaths, 
            const std::vector<uint32_t>& vUnits)
        {
            std::string errMsg = "success";

            if (vPaths.size() != vUnits.size())
            {
                errMsg = "prepare texture2d args error";
                return { std::move(errMsg), false };
            }

            std::unordered_set<uint32_t> records;
            records.reserve(vUnits.size());
            for (int i = 0; i < vUnits.size(); i++)
            {
                if (records.find(vUnits[i]) != records.end())
                {
                    errMsg = "prepare texture2d vUnits dup";
                    return { std::move(errMsg), false };
                }
                records.insert(vUnits[i]);
            }
            records.clear();

            for (int i = 0; i < vPaths.size(); i++)
            {
                auto t = std::make_unique<Texture2D>(vUnits[i]);
                auto [err, ok] = t->Load(vPaths[i]);
                if (!ok)
				{
                    errMsg = std::format("load texture2d error,{},{},{},{}", err, i, vPaths[i], vUnits[i]);
					return { std::move(errMsg), false };
				}
                m_texture2dMap[vUnits[i]].swap(t);
            }

            return { std::move(errMsg), true };
        }
    }
}
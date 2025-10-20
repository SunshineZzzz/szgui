#pragma once

#include <SDL3/SDL.h>
#include <GLES2/gl2.h>

#include <tuple>
#include <string>

#include "IRender.h"

namespace sz_gui 
{
    class GLESContext: public IRender
    {
    public:
        // GL属性设置
        static std::tuple<std::string, bool> InitGLESAttributes();

    public:
        GLESContext(SDL_Window* window);
        ~GLESContext();

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

    private:
        // SDL窗口指针
        SDL_Window* m_window = nullptr;
        // gl上下文
        SDL_GLContext m_glcontext = nullptr;
    };
}
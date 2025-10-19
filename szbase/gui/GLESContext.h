#pragma once

#include <SDL3/SDL.h>

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
        // 交换显卡内存缓冲区
        void SwapWindow();

    private:
        // SDL窗口指针
        SDL_Window* m_window = nullptr;
        // gl上下文
        SDL_GLContext m_glcontext = nullptr;
    };
}
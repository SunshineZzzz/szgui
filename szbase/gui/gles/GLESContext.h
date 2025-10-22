// comment: OpenGLES渲染器

#pragma once

#include <SDL3/SDL.h>
#include <GLES2/gl2.h>

#include <tuple>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "../IRender.h"
#include "Texture2D.h"

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
            // 准备2D纹理
            std::tuple<std::string, bool> PrepareTexture2D(const std::vector<std::string>& vPaths, 
                const std::vector<uint32_t>& vUnits) override;

        private:
            // SDL窗口指针
            SDL_Window* m_window = nullptr;
            // gles上下文
            SDL_GLContext m_glesContext = nullptr;
            // 纹理单元<->2D纹理对象
            std::map<uint32_t, std::unique_ptr<Texture2D>> m_texture2dMap;
        };
    }
}
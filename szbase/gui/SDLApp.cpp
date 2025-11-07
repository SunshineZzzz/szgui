#include "SDLApp.h"
#include "gl/GLContext.h"
#include "UIManager.h"

namespace sz_gui
{
    std::tuple<std::string, bool> SDLApp::InitSDL()
    {
        std::string errMsg = "success";

        #ifdef USE_OPENGL_ES
        if (!SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1"))
        {
            errMsg = "sdl es driver error," + std::string(SDL_GetError());
            return { std::move(errMsg), false };
        }
        #endif

        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            errMsg = "sdl init error," + std::string(SDL_GetError());
            return { std::move(errMsg), false };
        }

        auto [err, ok] = gl::GLContext::InitGLAttributes();
        if (!ok) 
        {
            return { std::move(errMsg), false };
        }

        return { std::move(errMsg), true };
    }

    SDLApp::SDLApp() {}

    SDLApp::~SDLApp()
    {
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        m_render = nullptr;

        SDL_Quit();
    }

    std::tuple<const std::string, bool> SDLApp::BuildTrueType(const std::string path)
    {
        std::string errMsg = "success";

		if (!m_window || !m_render || !m_uiManager)
		{
			errMsg = "not init sdl";
			return { std::move(errMsg), false };
		}

        auto [err, ok] = m_render->BuildTrueType(path);
		if (!ok)
		{
			return { std::move(err), false };
		}

		return { std::move(errMsg), true };
    }

    std::tuple<const std::string, bool> SDLApp::CreateWindow(
        const std::string& title, const int& width, const int& height)
    {
        std::string errMsg = "success";

        if (width <= 0 || height <= 0)
        {
            errMsg = "invalid window size";
            return { std::move(errMsg), false };
        }

        m_width = width;
        m_height = height;

        m_window = SDL_CreateWindow(title.c_str(), (int)width, (int)height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!m_window)
        {
            errMsg = "sdl create window error," + std::string(SDL_GetError());
            return { std::move(errMsg), false };
        }

        m_render = std::make_shared<gl::GLContext>(m_window);
        auto [err, ok] = m_render->Init(m_width, m_height);
        if (!ok)
		{
			return { std::move(err), false };
		}

        m_uiManager = std::make_shared<UIManager>(m_render);
        m_uiManager->Init(width, height);

        return { std::move(errMsg), true };
    }

    void SDLApp::Run()
    {
        if (!m_window || !m_render || !m_uiManager)
        {
            return;
        }

        bool running = true;
        SDL_Event event{};

        m_uiManager->RunBeforWork();

        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    running = false;
                }
                else if (event.type == SDL_EVENT_WINDOW_RESIZED)
                {
                    m_width = event.window.data1;
                    m_height = event.window.data2;
                    m_render->OnWindowResize(m_width, m_height);
                }
                m_uiManager->HandleEvent(&event);
            }
            m_uiManager->Render();
        }
    }

    void SDLApp::DoRender()
    {
        m_uiManager->Render();
    }

    bool SDLApp::RegToUI(std::shared_ptr<IUIBase> ui)
    {
        if (!m_window || !m_render || !m_uiManager)
        {
            return false;
        }

        return m_uiManager->RegTopUI(ui);
    }

    bool SDLApp::UnRegTopUI(std::shared_ptr<IUIBase> ui)
    {
        if (!m_window || !m_render || !m_uiManager)
        {
            return false;
        }

        return m_uiManager->UnRegTopUI(ui);
    }

    bool SDLApp::SetLayout(ILayout* pLyout)
    {
        if (!m_window || !m_render || !m_uiManager)
        {
            return false;
        }

        m_uiManager->SetLayout(pLyout);
        return true;
    }

    bool SDLApp::LayoutAddWidget(std::shared_ptr<IUIBase> widget)
    {
        if (!m_window || !m_render || !m_uiManager)
        {
            return false;
        }

		return m_uiManager->LayoutAddWidget(widget);
	}

	bool SDLApp::LayoutDelWidget(std::shared_ptr<IUIBase> widget)
	{
        if (!m_window || !m_render || !m_uiManager)
        {
            return false;
        }
        
		return m_uiManager->LayoutDelWidget(widget);
	}
}
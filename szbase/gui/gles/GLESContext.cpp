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

            // 保证析构顺序
            m_shaderMap.clear();
            m_shaderUnorderdmap.clear();
            m_texture2dMap.clear();

            m_drawCommandsVec.clear();
            m_vertexVec.clear();
            m_indicesVec.clear();

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

        std::tuple<std::string, bool> GLESContext::PrepareShaderImpl(const char* vertexShaderSource,
            const char* fragmentShaderSource, const char* name)
        {
            std::string errMsg = "success";

			if (!vertexShaderSource || !fragmentShaderSource)
			{
				errMsg = "prepare shader args error";
				return { std::move(errMsg), false };
			}

            if (m_shaderUnorderdmap.find(name) != m_shaderUnorderdmap.end())
			{
				errMsg = "prepare shader name dup";
				return { std::move(errMsg), false };
			}

			auto t = std::make_unique<Shader>();
            auto [err, ok] = t->LoadFromString(vertexShaderSource, fragmentShaderSource);
			if (!ok)
			{
				errMsg = std::format("load shader error,{},{},{}", err, vertexShaderSource, fragmentShaderSource);
				return { std::move(errMsg), false };
			}
            auto id = GenShaderId();
            auto result = m_shaderMap.emplace(id, std::move(t));
            m_shaderUnorderdmap[name] = result.first;

			return { std::move(errMsg), true };
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
                m_texture2dMap[vUnits[i]] = std::move(t);
            }

            return { std::move(errMsg), true };
        }

        void GLESContext::AppendDrawData(const std::vector<sz_ds::Vertex>& vertices,
            const std::vector<uint32_t>& indices, DrawCommand cmd)
        {
            // 1. 记录当前的全局大小作为偏移量
            const uint32_t vertexOffset = (uint32_t)m_vertexVec.size();
            const uint32_t indexOffset = (uint32_t)m_indicesVec.size();

            // 2. 将顶点数据追加到全局顶点集合
            m_vertexVec.reserve(m_vertexVec.size() + vertices.size());
            m_vertexVec.insert(m_vertexVec.end(), std::make_move_iterator(vertices.begin()), 
                std::make_move_iterator(vertices.end()));

            // 3. 将索引数据追加到全局索引集合，并进行索引偏移修正 (关键步骤!)
            m_indicesVec.reserve(m_indicesVec.size() + indices.size());
            for (uint32_t index : indices) 
            {
                // 将局部索引值 0, 1, 2... 修正为全局索引值 (0+offset, 1+offset, 2+offset...)
                m_indicesVec.push_back(index + vertexOffset);
            }

            // 4. 填充 DrawCommand 的几何引用信息
            cmd.m_vertexOffset = vertexOffset;
            cmd.m_indexOffset = indexOffset;
            cmd.m_indexCount = (uint32_t)indices.size();

            // 5. 将完整的 DrawCommand 加入命令集合
            m_drawCommandsVec.push_back(std::move(cmd));
        }
    }
}
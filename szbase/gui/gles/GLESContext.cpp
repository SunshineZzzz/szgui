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
                !SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) ||
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

            const char* vs =
            {
                "#version 300 es\n"
                "layout(location = 0) in vec3 aPos;\n"
                "layout(location = 1) in vec4 aColor;\n"
                "layout(location = 2) in vec2 aUV;\n"
                "out vec4 color;\n"
                "out vec2 uv;\n"
                "uniform mat4 modelMatrix;\n"
                "uniform mat4 viewMatrix;\n"
                "uniform mat4 projectionMatrix;\n"
                "void main()\n"
                "{\n"
                "   vec4 position = vec4(aPos, 1.0);\n"
                "   position = projectionMatrix * viewMatrix * modelMatrix * position;\n"
                "   gl_Position = position;\n"
                "   color = aColor;\n"
                "   uv = aUV;\n"
                "}\n"
            };
            const char* ps =
            {
                "#version 300 es\n"
                "out vec4 FragColor;\n"
                "in vec4 color;\n"
                "in vec2 uv;\n"
                "uniform sampler2D sampler;\n"
                "uniform bool useColor;\n"
                "uniform bool isText;\n"
                "void main()\n"
                "{\n"
                "   if (useColor)"
                "   {\n"
                "        FragColor = color;\n"
                "        return;\n"
                "   }\n"
                "	vec4 texColor = texture(sampler, uv);\n"
                "   if (!isText)\n"
                "   {\n"
                "	    FragColor = texColor;\n"
                "       return;\n"
                "   }\n"
                "	FragColor = vec4(color.rgb * texColor.a, color.a * texColor.a);\n"
                "}\n"
            };
            auto [err, ok] = PrepareShader(vs, ps);
            if (!ok)
			{
				errMsg = std::format("prepare shader error,{}", err);
				return { std::move(errMsg), false };
			}
            
            // 开启深度检测
            EnableRenderState(RenderState::EnableDepthTest);
            // 开启混合
            EnableRenderState(RenderState::EnableBlend);

            return { std::move(errMsg), true };
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

        void GLESContext::PushScissor(const sz_ds::Rect& rect)
        {
            sz_ds::AABB2D intersection;

            if (m_scissorStack.empty())
            {
                intersection = GetViewportAndScissorRectIntersection(rect);
            }
            else
            {
                sz_ds::Rect parentRect = m_scissorStack.top();
                intersection = parentRect.ToAABB2D().Intersection(rect.ToAABB2D());
            }

            auto newScissorRect = intersection.GetRect();
            m_scissorStack.push(newScissorRect);
            EnableRenderState(RenderState::EnableScissorTest, newScissorRect);
        }

        void GLESContext::PopScissor()
        {
            assert(m_scissorStack.size() != 0);

            m_scissorStack.pop();

            if (m_scissorStack.empty())
            {
                DisableRenderState(RenderState::EnableScissorTest);
                return;
            }

            const auto& parentRect = m_scissorStack.top();
            EnableRenderState(RenderState::EnableScissorTest, parentRect);
        }

        void GLESContext::FullDraw()
        {
            const auto [width, height] = GetWindowSize();
            // 清理
            Clear();
            // 设置视口
            SetViewPort(0, 0, width, height);
            // 准备摄像机    
            PrepareCamera(GetWindowSize().first, GetWindowSize().second);


        }

        void GLESContext::IncDraw()
        {

        }

        void GLESContext::SetViewPort(int x, int y, int width, int height)
        {
            glViewport(x, y, width, height);
        }

        void GLESContext::SetClearColor(float red, float green, float blue, float alpha)
        {
            glClearColor(red, green, blue, alpha);
        }

        void GLESContext::Clear()
        {
            // 清除颜色缓冲区(被清理为glClearColor设置的颜色)|清理深度缓冲区(被清理为1.0)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        bool GLESContext::SwapWindow()
        {
            return SDL_GL_SwapWindow(m_window);
        }

        void GLESContext::EnableRenderState(RenderState state, std::any data)
        {
            switch (state)
            {
            case RenderState::EnableScissorTest:
            {
                const sz_ds::Rect* rectPtr = std::any_cast<sz_ds::Rect>(&data);
                if (!rectPtr)
                {
                    assert(0);
                    return;
                }

                if (HasFlag(m_curRenderState, RenderState::EnableScissorTest) && m_curScissorRect == *rectPtr)
				{
					return;
				}

                glEnable(GL_SCISSOR_TEST);
                m_curScissorRect = *rectPtr;
                // 设置裁剪区域
                glScissor((int)rectPtr->m_x, (int)rectPtr->m_y, (int)rectPtr->m_width, (int)rectPtr->m_height);
            }
			break;
            case RenderState::EnableDepthTest:
            {
                if (HasFlag(m_curRenderState, RenderState::EnableDepthTest))
				{
					return;
				}

                // 开启深度检测
                glEnable(GL_DEPTH_TEST);
                // 深度检测的比较函数, GL_LESS: 当前片元深度值小于当前深度缓冲区中深度值时通过测试
                glDepthFunc(GL_GREATER);
            }
            break;
            case RenderState::EnableBlend:
			{
                if (HasFlag(m_curRenderState, RenderState::EnableBlend))
                {
                    return;
                }

                // 开启混合
                glEnable(GL_BLEND);
                // 计算源像素(Source，新绘制的)和目标像素(Destination，颜色缓冲区中已有的的颜色混合方式，
                // Final_Color = Source_Color x Source_Alpha + Destination_Color x (1 - Source_Alpha)
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
            break;
			default:
                return;
			}

            m_curRenderState = m_curRenderState | state;
        }

        void GLESContext::DisableRenderState(RenderState state)
        {
            switch (state)
            {
            case RenderState::EnableScissorTest:
            {
                if (!HasFlag(m_curRenderState, RenderState::EnableScissorTest))
                {
                    return;
                }
                glDisable(GL_SCISSOR_TEST);
                m_curScissorRect = sz_ds::Rect();
            }
            break;
            case RenderState::EnableDepthTest:
            {
                if (!HasFlag(m_curRenderState, RenderState::EnableDepthTest))
                {
					return;
				}
                glDisable(GL_DEPTH_TEST);
            } 
            break;
            case RenderState::EnableBlend:
            {
                if (!HasFlag(m_curRenderState, RenderState::EnableBlend))
                {
                    return;
                }
                glDisable(GL_BLEND);
            }
			break;
			default:
                return;
			}

            m_curRenderState = m_curRenderState & ~state;
        }

        void GLESContext::BeginUseShader(uint32_t shaderId)
        {
            if (shaderId == 0)
			{
				return;
			}

            if (m_curShaderId == shaderId)
			{
				return;
			}

			EndUserShader();
			
			m_curShaderId = shaderId;
			m_shaderMap[shaderId]->Begin();
        }

        void GLESContext::EndUserShader()
        {
            if (m_curShaderId == 0)
			{
				return;
			}

            m_shaderMap[m_curShaderId]->End();
            m_curShaderId = 0;
        }
        
        void GLESContext::BeginUseTexture2D(uint32_t texture2dId)
        {
            if (texture2dId == 0)
            {
                return;
            }

            if (m_curTexture2dId == texture2dId)
			{
				return;
			}

			m_curTexture2dId = texture2dId;
			m_texture2dMap[texture2dId]->Bind();
        }
    }
}
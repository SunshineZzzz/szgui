#include "GLContext.h"
#include "Geometry.h"
#include "CheckRstErr.h"
#include "ShaderDefine.h"

#include <unordered_set>
#include <format>
#include <algorithm>

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
            //if (!SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1)||
            //    !SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4))
            //{
            //    errMsg = "gl set multisample error," + std::string(SDL_GetError());
            //    return { std::move(errMsg), false };
            //}

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

        std::tuple<std::string, bool> GLContext::Init(int width, int height)
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

            m_colorShader = std::make_unique<Shader>();
            auto [err, ok] = m_colorShader->LoadFromString(ColorVS, ColorFS);
            if (!ok)
			{
				return { err, false };
			}

            GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
            
            OnWindowResize(width, height);

            return { std::move(errMsg), true };
        }

        void GLContext::AppendDrawData(const glm::vec3& pos, 
            const std::vector<float>& positions, const std::vector<float>& colorOrUVs,
            const std::vector<uint32_t>& indices, DrawCommand cmd)
        { 
            auto ri = std::make_unique<RenderItem>();
            ri->m_position = pos;
            ri->m_drawMode = getDrawMode(cmd.m_drawMode);
            ri->m_geo = std::make_unique<Geometry>();
            if (cmd.m_materialType == MaterialType::ColorMaterial)
            {
                ri->m_geo->UploadColors(positions, colorOrUVs, indices);
            }
            else if (cmd.m_materialType == MaterialType::TextureMaterial)
            {
            }
            else if (cmd.m_materialType == MaterialType::TextMaterial)
            {
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableFaceCulling))
            {
                ri->m_cullFace = true;
                ri->m_frontFace = (cmd.m_faceCulling.m_frontFace ==
                    FrontFaceType::CCW ? GL_CCW : GL_CW);
                ri->m_cullFace = (cmd.m_faceCulling.m_cullFace ==
                    CullFaceType::Back ? GL_BACK : GL_FRONT);
            }
            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableScissorTest))
            {
            }
            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableDepthTest))
            {
                ri->m_depthTest = true;
                ri->m_depthFunc = getDepthFunc(cmd.m_depthTest.m_depthFunc);
                ri->m_depthWrite = cmd.m_depthTest.m_depthWrite;
                if (ri->m_blend)
                {
                    ri->m_depthWrite = false;
                }
            }
            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableBlend))
            {
                ri->m_blend = true;
                ri->m_sFactor = getBlendSFactor(cmd.m_blend.m_srcBlendFunc);
                ri->m_dFactor = getBlendDFactor(cmd.m_blend.m_dstBlendFunc);
                ri->m_opacity = cmd.m_blend.m_opacity;
            }

            if (ri->m_blend)
            {
                m_transparentObjects.push_back(std::move(ri));
                return;
            }
            m_opacityObjects.push_back(std::move(ri));
        }

        void GLContext::Render()
        {
            // 设置当前帧，绘制的时候，opengl的必要状态机参数
            // 默认开启面剔除
            GL_CALL(glEnable(GL_CULL_FACE));
            GL_CALL(glFrontFace(GL_CCW));
            GL_CALL(glCullFace(GL_BACK));
            
            // 默认开启深度测试
            GL_CALL(glEnable(GL_DEPTH_TEST));
            GL_CALL(glDepthFunc(GL_LESS));
            GL_CALL(glDepthMask(GL_TRUE));

            // 默认关闭颜色混合
            GL_CALL(glDisable(GL_BLEND));

            // 清理画布 
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            // 先绘制不透明物体，透明物体按照距离摄像机远近排序，由远到近绘制
            std::sort(
                m_transparentObjects.begin(),
                m_transparentObjects.end(),
                [this](const std::unique_ptr<RenderItem>& a, const std::unique_ptr<RenderItem>& b) {
                    auto viewMatrix = m_camera->GetViewMatrix();

                    // 计算a的相机系的Z
                    auto modelMatrixA = a->GetModelMatrix();
                    auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);
                    auto cameraPositionA = viewMatrix * worldPositionA;

                    // 计算b的相机系的Z
                    auto modelMatrixB = b->GetModelMatrix();
                    auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);
                    auto cameraPositionB = viewMatrix * worldPositionB;

                    return cameraPositionA.z > cameraPositionB.z;
                }
            );

            // 先绘制不透明物体
            for (int i = 0; i < m_opacityObjects.size(); i++)
            {
                renderObject(m_opacityObjects[i]);
            }

            // 透明物体按照距离摄像机远近排序，由远到近绘制
            for (int i = 0; i < m_transparentObjects.size(); i++)
            {
                renderObject(m_transparentObjects[i]);
            }

            SDL_GL_SwapWindow(m_window);

            m_transparentObjects.clear();
            m_opacityObjects.clear();
        }

        GLenum GLContext::getDrawMode(DrawMode mode)
        {
            switch (mode)
            {
            case DrawMode::LINE_LOOP:
                return GL_LINE_LOOP;
            default:
                assert(0);
            }
            return GL_TRIANGLES;
        }

        void GLContext::renderObject(const std::unique_ptr<RenderItem>& ri)
        {
            // 设置渲染状态
            setFaceCullingState(ri);
            setDepthState(ri);
            setBlendState(ri);
            
            // 决定使用哪个Shader 
            auto& shader = pickShader(ri->m_type);
            shader->Begin();

            switch (ri->m_type)
            {
            case MaterialType::ColorMaterial:
                // mvp
                shader->SetUniformMatrix4x4("modelMatrix", ri->GetModelMatrix());
                shader->SetUniformMatrix4x4("viewMatrix", m_camera->GetViewMatrix());
                shader->SetUniformMatrix4x4("projectionMatrix", m_camera->GetProjectionMatrix());
				break;
			case MaterialType::TextureMaterial:
				break;
			case MaterialType::TextMaterial:
				break;
			default:
                assert(0);
            }

            // 绑定vao
            GL_CALL(glBindVertexArray(ri->m_geo->GetVao()));
            // 绘制
            GL_CALL(glDrawElements(ri->m_drawMode, (GLsizei)ri->m_geo->GetIndicesCount(), GL_UNSIGNED_INT, 0));
        }

        std::unique_ptr<Shader>& GLContext::pickShader(MaterialType type)
        {
            switch (type)
            {
            case MaterialType::ColorMaterial:
                return m_colorShader;          
            }

            return m_errShader;
        }

        GLenum GLContext::getBlendSFactor(BlendFuncType type)
        {
            switch (type)
            {
            case BlendFuncType::SRC_ALPHA:
                return GL_SRC_ALPHA;
            default:
                return GL_SRC_ALPHA;
            }
        }

        GLenum GLContext::getBlendDFactor(BlendFuncType type)
        {
            switch (type)
            {
            case BlendFuncType::ONE_MINUS_SRC_ALPHA:
                return GL_ONE_MINUS_SRC_ALPHA;
            default:
                return GL_ONE_MINUS_SRC_ALPHA;
            }
        }

        GLenum GLContext::getDepthFunc(DepthFuncType type)
        {
            switch (type)
			{
            case DepthFuncType::Less:
				return GL_LESS;
            case DepthFuncType::Greater:
                return GL_GREATER;
			default:
				return GL_GREATER;
			}
        }
        
        void GLContext::setFaceCullingState(const std::unique_ptr<RenderItem>& ri)
        {
            if (ri->m_faceCulling)
            {
                GL_CALL(glEnable(GL_CULL_FACE));
                GL_CALL(glFrontFace(ri->m_frontFace));
                GL_CALL(glCullFace(ri->m_cullFace));
            }
            else
            {
                GL_CALL(glDisable(GL_CULL_FACE));
            }
        }

        void GLContext::setDepthState(const std::unique_ptr<RenderItem>& ri)
        {
            if (ri->m_depthTest)
            {
                GL_CALL(glEnable(GL_DEPTH_TEST));
                GL_CALL(glDepthFunc(ri->m_depthFunc));
            }
            else
            {
                GL_CALL(glDisable(GL_DEPTH_TEST));
            }

            if (ri->m_depthWrite)
            {
                GL_CALL(glDepthMask(GL_TRUE));
            }
            else 
            {
                GL_CALL(glDepthMask(GL_FALSE));
            }
        }

        void GLContext::setBlendState(const std::unique_ptr<RenderItem>& ri)
        {
            if (ri->m_blend)
            {
                GL_CALL(glEnable(GL_BLEND));
                GL_CALL(glBlendFunc(ri->m_sFactor, ri->m_dFactor));
            }
            else
            {
                GL_CALL(glDisable(GL_BLEND));
            }
        }
    }
}
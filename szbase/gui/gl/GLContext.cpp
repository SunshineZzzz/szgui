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

        void GLContext::AppendDrawData(const std::vector<float>& positions, 
            const std::vector<float>& colorOrUVs,
            const std::vector<uint32_t>& indices, DrawCommand cmd)
        { 
            assert(cmd.m_onlyId);

            RenderItem* ri = nullptr;
            bool oldOpcacity = false;
            bool oldTransparent = false;

            auto oIt = m_opacityUnmap.find(cmd.m_onlyId);
            auto tIt = m_transparentUnmap.find(cmd.m_onlyId);
            if (oIt == m_opacityUnmap.end() && tIt == m_transparentUnmap.end())
            {
                ri = new RenderItem();
                bool useColor = (cmd.m_materialType == MaterialType::ColorMaterial);
                ri->m_geo = std::make_unique<Geometry>(
                    positions.size() * sizeof(float), 
                    colorOrUVs.size() * sizeof(float), 
                    indices.size() * sizeof(uint32_t), useColor);
            }
            else if (oIt == m_opacityUnmap.end())
            {
                oldTransparent = true;
                ri = tIt->second->get();
            }
            else
            {
                oldOpcacity = true;
                ri = oIt->second->get();
            }

            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableBlend) && oldOpcacity)
            {
                auto oldRenderItem = std::move(*(oIt->second));
                m_opacityItems.erase(oIt->second);
                m_opacityUnmap.erase(oIt);

                auto it = m_transparentItems.insert(m_transparentItems.end(), std::move(oldRenderItem));
                m_transparentUnmap[cmd.m_onlyId] = it;
                oldOpcacity = false;
                oldTransparent = true;
            }

            ri->m_position = cmd.m_worldPos;
            ri->m_drawMode = getDrawMode(cmd.m_drawMode);
            uploadToGPU(ri, positions, colorOrUVs, indices, cmd);
            
            if (sz_utils::HasFlag(cmd.m_renderState, RenderState::EnableFaceCulling))
            {
                ri->m_cullFace = true;
                ri->m_frontFace = (cmd.m_faceCulling.m_frontFace ==
                    FrontFaceType::CCW ? GL_CCW : GL_CW);
                ri->m_cullFace = (cmd.m_faceCulling.m_cullFace ==
                    CullFaceType::Back ? GL_BACK : GL_FRONT);
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

            if (oldOpcacity || oldTransparent)
            {
                return;
            }

            if (ri->m_blend)
            {
                m_transparentItems.push_back(std::unique_ptr<RenderItem>(ri));
                m_transparentUnmap[cmd.m_onlyId] = std::prev(m_transparentItems.end());
                return;
            }

            m_opacityItems.push_back(std::unique_ptr<RenderItem>(ri));
			m_opacityUnmap[cmd.m_onlyId] = std::prev(m_opacityItems.end());
        }

        void GLContext::uploadToGPU(RenderItem* ri, const std::vector<float>& positions,
            const std::vector<float>& colorOrUVs, const std::vector<uint32_t>& indices,
            DrawCommand cmd)
        {
            if (cmd.m_uploadOp == UploadOperation::Retain)
            {
                return;
            }

            if (cmd.m_materialType == MaterialType::ColorMaterial)
            {
                if (cmd.m_uploadOp == (UploadOperation::UploadPos |
                    UploadOperation::UploadColorOrUv | UploadOperation::UploadIndex))
                {
                    ri->m_geo->UploadAll(positions, colorOrUVs, indices);
					return;
                }

                if (sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadPos))
				{
					ri->m_geo->UploadPositions(positions);
				}
				if (sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadColorOrUv))
				{
					ri->m_geo->UploadColorsOrUVs(colorOrUVs);
				}
				if (sz_utils::HasFlag(cmd.m_uploadOp, UploadOperation::UploadIndex))
				{
					ri->m_geo->UploadIndices(indices);
				}
            }

            if (cmd.m_materialType == MaterialType::TextureMaterial)
            {
                assert(0);
            }

            if (cmd.m_materialType == MaterialType::TextMaterial)
            {
                assert(0);
            }
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
            m_transparentItems.sort(
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

                    return cameraPositionA.z < cameraPositionB.z;
                }
            );


            // 先绘制不透明物体
            for (const auto& item : m_opacityItems)
            {
                renderObject(item);
            }

            // 透明物体按照距离摄像机远近排序，由远到近绘制
            for (const auto& item : m_transparentItems)
            {
                renderObject(item);
            }

            SDL_GL_SwapWindow(m_window);
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
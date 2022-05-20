#pragma once

#include "Renderer/RendererAPI.h"

namespace Venus {

	class RenderCommand
	{
		public:
			static void Init()
			{
				s_RendererAPI->Init();
			}

			static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
			{
				s_RendererAPI->SetViewport(x, y, width, height);
			}

			static void SetClearColor(const glm::vec4& color)
			{
				s_RendererAPI->SetClearColor(color);
			}

			static void Clear()
			{
				s_RendererAPI->Clear();
			}

			static void DisableStencilTest()
			{
				s_RendererAPI->DisableStencilTest();
			}

			static void EnableStencilTest()
			{
				s_RendererAPI->EnableStencilTest();
			}

			static void DisableStencilWrite()
			{
				s_RendererAPI->DisableStencilWrite();
			}

			static void EnableStencilWrite()
			{
				s_RendererAPI->EnableStencilWrite();
			}

			static void DisableDepthTest()
			{
				s_RendererAPI->DisableDepthTest();
			}

			static void EnableDepthTest()
			{
				s_RendererAPI->EnableDepthTest();
			}

			static void SetStencilTest(int function, int value, int mask)
			{
				s_RendererAPI->SetStencilTest(function, value, mask);
			}

			static void SetLineWidth(float width)
			{
				s_RendererAPI->SetLineWidth(width);
			}

			// TEMP
			static void BindTexture(int textureID)
			{
				s_RendererAPI->BindTexture(textureID);
			}
			
			// TEMP
			static void BindFramebuffer(int framebufferID)
			{
				s_RendererAPI->BindFramebuffer(framebufferID);
			}

			static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
			{
				s_RendererAPI->DrawIndexed(vertexArray, indexCount);
			}

			static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
			{
				s_RendererAPI->DrawLines(vertexArray, vertexCount);
			}

		private:
			static Scope<RendererAPI> s_RendererAPI;
	};

}

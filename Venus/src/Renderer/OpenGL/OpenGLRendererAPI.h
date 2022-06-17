#pragma once

#include "Renderer/RendererAPI.h"

namespace Venus {

	class OpenGLRendererAPI : public RendererAPI
	{
		public:
			virtual void Init() override;
			virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

			virtual void SetClearColor(const glm::vec4& color) override;
			virtual void Clear() override;

			virtual void DisableStencilTest() override;
			virtual void EnableStencilTest() override;
			virtual void DisableStencilWrite() override;
			virtual void EnableStencilWrite() override;
			virtual void DisableDepthTest() override;
			virtual void EnableDepthTest() override;
			virtual void SetStencilTest(int function, int value, int mask) override;

			virtual void BindTexture(int location, int textureID) override;
			virtual void BindTextureCube(int location, int textureID) override;
			virtual void BindTextureArray(int location, int textureID) override;
			virtual void BindFramebuffer(int framebufferID) override;

			virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
			virtual void DrawArrays(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;
			virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) override;

			virtual void SetLineWidth(float width) override;
	};


}

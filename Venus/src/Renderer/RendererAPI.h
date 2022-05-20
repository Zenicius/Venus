#pragma once

#include <glm/glm.hpp>

#include "Renderer/VertexArray.h"

namespace Venus {

	class RendererAPI
	{
		public:
			enum class API
			{
				None = 0, OpenGL = 1
			};

		public:
			virtual ~RendererAPI() = default;

			virtual void Init() = 0;
			virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
			virtual void SetClearColor(const glm::vec4& color) = 0;
			virtual void Clear() = 0;

			virtual void DisableStencilTest() = 0;
			virtual void EnableStencilTest() = 0;
			virtual void DisableStencilWrite() = 0;
			virtual void EnableStencilWrite() = 0;
			virtual void DisableDepthTest() = 0;
			virtual void EnableDepthTest() = 0;
			virtual void SetStencilTest(int function, int value, int mask) = 0;

			virtual void SetLineWidth(float width) = 0;
			
			virtual void BindTexture(int textureID) = 0;
			virtual void BindFramebuffer(int framebufferID) = 0;

			virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
			virtual void DrawLines(const Ref<VertexArray>& vertexArrray, uint32_t vertexCount) = 0;

			static API GetAPI() { return s_API; }
			static std::string GetAPIName();
			static Scope<RendererAPI> Create();

		private:
			static API s_API;
	};

}

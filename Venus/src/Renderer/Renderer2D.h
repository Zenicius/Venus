#pragma once

#include "Renderer/OrthographicCamera.h"

#include "Renderer/Texture.h"

#include "Renderer/Camera.h"
#include "Renderer/EditorCamera.h"

#include "Scene/Components.h"

namespace Venus {

	class Renderer2D
	{
		public:
			static void Init();
			static void Shutdown();

			static void BeginScene(const glm::mat4& viewProj, const glm::mat4& view);
			static void EndScene();
			static void Flush();

			static void SetRenderTarget(Ref<Framebuffer> target);

			// Primitives
			static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
			static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
			static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

			static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
			static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

			static void DrawQuadBillboard(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

			static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
			static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
			static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
			
			static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
			static void DrawCircle(const glm::vec2& position, const glm::vec2& scale, const glm::vec4 color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

			static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);

			static void DrawRect(const glm::mat4& transform, const glm::vec4 color, int entityID = -1);
			static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4 color, int entityID = -1);

			static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);

			static float GetLineWidth();
			static void SetLineWitdh(float width);

			// Stats
			struct Statistics
			{
				uint32_t DrawCalls = 0;
				uint32_t QuadCount = 0;

				uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
				uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
			};
			static void ResetStats();
			static Statistics GetStats();

		private:
			static void StartBatch();
			static void NextBatch();

			friend class Renderer;
		};

}

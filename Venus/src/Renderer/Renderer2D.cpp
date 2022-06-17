#include "pch.h"
#include "Renderer/Renderer2D.h"

#include "Renderer/Renderer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Venus {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
		
		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		// Quads 
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		// Circles
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		// Lines
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		// Textures
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = Default white texture

		glm::vec4 QuadVertexPositions[4];

		// Stats
		Renderer2D::Statistics Stats;
	};

	static Renderer2DData s_2DData;

	void Renderer2D::Init()
	{
		VS_PROFILE_FUNCTION();

		// Quads
		s_2DData.QuadVertexArray = VertexArray::Create();

		s_2DData.QuadVertexBuffer = VertexBuffer::Create(s_2DData.MaxVertices * sizeof(QuadVertex));
		s_2DData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
		});
		s_2DData.QuadVertexArray->AddVertexBuffer(s_2DData.QuadVertexBuffer);

		s_2DData.QuadVertexBufferBase = new QuadVertex[s_2DData.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_2DData.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_2DData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_2DData.MaxIndices);
		s_2DData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;
		
		// Circles
		s_2DData.CircleVertexArray = VertexArray::Create();
		
		s_2DData.CircleVertexBuffer = VertexBuffer::Create(s_2DData.MaxVertices * sizeof(CircleVertex));
		
		s_2DData.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
		});

		s_2DData.CircleVertexArray->AddVertexBuffer(s_2DData.CircleVertexBuffer);
		s_2DData.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
		s_2DData.CircleVertexBufferBase = new CircleVertex[s_2DData.MaxVertices];

		// Lines
		s_2DData.LineVertexArray = VertexArray::Create();

		s_2DData.LineVertexBuffer = VertexBuffer::Create(s_2DData.MaxVertices * sizeof(LineVertex));

		s_2DData.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"		},
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Int,    "a_EntityID"      }
		});

		s_2DData.LineVertexArray->AddVertexBuffer(s_2DData.LineVertexBuffer);
		s_2DData.LineVertexBufferBase = new LineVertex[s_2DData.MaxVertices];

		// Textures
		int32_t samplers[s_2DData.MaxTextureSlots];
		for (uint32_t i = 0; i < s_2DData.MaxTextureSlots; i++)
			samplers[i] = i;

		// Shaders
		s_2DData.QuadShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Quad");
		s_2DData.CircleShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Circle");
		s_2DData.LineShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Line");

		// Set first texture slot to 0
		s_2DData.TextureSlots[0] = Renderer::GetDefaultTexture();

		s_2DData.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_2DData.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_2DData.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_2DData.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		VS_PROFILE_FUNCTION();

		delete[] s_2DData.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene()
	{
		VS_PROFILE_FUNCTION();

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		VS_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_2DData.QuadIndexCount = 0;
		s_2DData.QuadVertexBufferPtr = s_2DData.QuadVertexBufferBase;

		s_2DData.CircleIndexCount = 0;
		s_2DData.CircleVertexBufferPtr = s_2DData.CircleVertexBufferBase;

		s_2DData.LineVertexCount = 0;
		s_2DData.LineVertexBufferPtr = s_2DData.LineVertexBufferBase;

		s_2DData.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (s_2DData.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2DData.QuadVertexBufferPtr - (uint8_t*)s_2DData.QuadVertexBufferBase);
			s_2DData.QuadVertexBuffer->SetData(s_2DData.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_2DData.TextureSlotIndex; i++)
				s_2DData.TextureSlots[i]->Bind(i);

			s_2DData.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_2DData.QuadVertexArray, s_2DData.QuadIndexCount);
			s_2DData.Stats.DrawCalls++;
		}

		if (s_2DData.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2DData.CircleVertexBufferPtr - (uint8_t*)s_2DData.CircleVertexBufferBase);
			s_2DData.CircleVertexBuffer->SetData(s_2DData.CircleVertexBufferBase, dataSize);

			s_2DData.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_2DData.CircleVertexArray, s_2DData.CircleIndexCount);
			s_2DData.Stats.DrawCalls++;
		}

		if (s_2DData.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_2DData.LineVertexBufferPtr - (uint8_t*)s_2DData.LineVertexBufferBase);
			s_2DData.LineVertexBuffer->SetData(s_2DData.LineVertexBufferBase, dataSize);

			s_2DData.LineShader->Bind();
			RenderCommand::SetLineWidth(s_2DData.LineWidth);
			RenderCommand::DrawLines(s_2DData.LineVertexArray, s_2DData.LineVertexCount);
			s_2DData.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		VS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		VS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		VS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_2DData.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_2DData.QuadVertexBufferPtr->Position = transform * s_2DData.QuadVertexPositions[i];
			s_2DData.QuadVertexBufferPtr->Color = color;
			s_2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2DData.QuadVertexBufferPtr->EntityID = entityID;
			s_2DData.QuadVertexBufferPtr++;
		}

		s_2DData.QuadIndexCount += 6;

		s_2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		VS_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_2DData.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_2DData.TextureSlotIndex; i++)
		{
			if (*s_2DData.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_2DData.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_2DData.TextureSlotIndex;
			s_2DData.TextureSlots[s_2DData.TextureSlotIndex] = texture;
			s_2DData.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_2DData.QuadVertexBufferPtr->Position = transform * s_2DData.QuadVertexPositions[i];
			s_2DData.QuadVertexBufferPtr->Color = tintColor;
			s_2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_2DData.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_2DData.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_2DData.QuadVertexBufferPtr->EntityID = entityID;
			s_2DData.QuadVertexBufferPtr++;
		}

		s_2DData.QuadIndexCount += 6;

		s_2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		VS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		VS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness, float fade, int entityID)
	{
		VS_PROFILE_FUNCTION();

		for (size_t i = 0; i < 4; i++)
		{
			s_2DData.CircleVertexBufferPtr->WorldPosition = transform * s_2DData.QuadVertexPositions[i];
			s_2DData.CircleVertexBufferPtr->LocalPosition = s_2DData.QuadVertexPositions[i] * 2.0f;
			s_2DData.CircleVertexBufferPtr->Color = color;
			s_2DData.CircleVertexBufferPtr->Thickness = thickness;
			s_2DData.CircleVertexBufferPtr->Fade = fade;
			s_2DData.CircleVertexBufferPtr->EntityID = entityID;
			s_2DData.CircleVertexBufferPtr++;
		}

		s_2DData.CircleIndexCount += 6;

		s_2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawCircle(const glm::vec2& position, const glm::vec2& scale, const glm::vec4 color, float thickness, float fade, int entityID)
	{
		VS_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position, 0.0f })
			* glm::scale(glm::mat4(1.0f), { scale.x, scale.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			s_2DData.CircleVertexBufferPtr->WorldPosition = transform * s_2DData.QuadVertexPositions[i];
			s_2DData.CircleVertexBufferPtr->LocalPosition = s_2DData.QuadVertexPositions[i] * 2.0f;
			s_2DData.CircleVertexBufferPtr->Color = color;
			s_2DData.CircleVertexBufferPtr->Thickness = thickness;
			s_2DData.CircleVertexBufferPtr->Fade = fade;
			s_2DData.CircleVertexBufferPtr->EntityID = entityID;
			s_2DData.CircleVertexBufferPtr++;
		}

		s_2DData.CircleIndexCount += 6;

		s_2DData.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		VS_PROFILE_FUNCTION();

		s_2DData.LineVertexBufferPtr->Position = p0;
		s_2DData.LineVertexBufferPtr->Color = color;
		s_2DData.LineVertexBufferPtr->EntityID = entityID;
		s_2DData.LineVertexBufferPtr++;

		s_2DData.LineVertexBufferPtr->Position = p1;
		s_2DData.LineVertexBufferPtr->Color = color;
		s_2DData.LineVertexBufferPtr->EntityID = entityID;
		s_2DData.LineVertexBufferPtr++;

		s_2DData.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4 color, int entityID)
	{
		VS_PROFILE_FUNCTION();

		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);	
		DrawLine(p3, p0, color);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4 color, int entityID)
	{
		VS_PROFILE_FUNCTION();

		glm::vec3 lineVertices[4];
		
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_2DData.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_2DData.LineWidth;
	}

	void Renderer2D::SetLineWitdh(float width)
	{
		s_2DData.LineWidth = width;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_2DData.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_2DData.Stats;
	}

}

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"
#include "OpenGL/VAO.h"
#include "OpenGL/VBO.h"

namespace Venus {

	class Renderer2D
	{
		public:
			Renderer2D(Shader& shader, Shader& textShader);
			~Renderer2D();

			void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f),
				float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

			void DrawText(const std::string& text, float x, float y, float scale, glm::vec3 color);

		private:
			Shader m_Shader, m_TextShader;
			VAO m_VAO, m_TextVAO;
			VBO m_TextVBO;

			void InitRender();
			void InitRenderText();
	};
}

// COLORS
#define COLOR_WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define COLOR_BLACK glm::vec3(0.0f, 0.0f, 0.0f)
#define COLOR_RED glm::vec3(1.0f, 0.0f, 0.0f)
#define COLOR_BLUE glm::vec3(0.0f, 0.0f, 1.0f)
#define COLOR_GREEN glm::vec3(0.0, 1.0f, 0.0f)



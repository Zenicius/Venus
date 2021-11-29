#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Engine/ResourceManager.h"
#include "OrthographicCamera.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"
#include "OpenGL/VAO.h"
#include "OpenGL/VBO.h"

namespace Venus {

	class Renderer2D
	{
		public:
			static void Init(Shader& shader, Shader& textShader);
			static void Shutdown();

			static void SetClearColor(glm::vec3 color);
			static void Clear();

			static void StartScene(OrthographicCamera& camera);
			static void DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f),
				float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
			static void DrawText(const std::string& text, float x, float y, float scale, glm::vec3 color);

		private:
			static void Init2D();
			static void InitText();
	};
}

// COLORS
#define COLOR_WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define COLOR_BLACK glm::vec3(0.0f, 0.0f, 0.0f)
#define COLOR_RED glm::vec3(1.0f, 0.0f, 0.0f)
#define COLOR_BLUE glm::vec3(0.0f, 0.0f, 1.0f)
#define COLOR_GREEN glm::vec3(0.0, 1.0f, 0.0f)



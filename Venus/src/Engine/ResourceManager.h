#pragma once

#include "glm/glm.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "Renderer/OpenGL/Shader.h"
#include "Renderer/OpenGL/Texture.h"

namespace Venus {

	struct Character
	{
		unsigned int Texture;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		unsigned int Advance;
	};

	class ResourceManager
	{
		public:
			static Shader LoadShader(const std::string& vertex, const std::string& fragment, const std::string& name);
			static Shader& GetShader(const std::string& name);

			static Texture LoadTexture(const std::string& fileName, const std::string& texName);
			static Texture& GetTexture(const std::string& name);

			static void LoadFont(const std::string& font, unsigned int fontSize);
			static std::map<char, Character>& GetFont();

			static void Clear();

		private:
			ResourceManager() {}
			static std::unordered_map<std::string, Shader> ShaderLibrary;
			static std::unordered_map<std::string, Texture> TextureLibrary;
			static std::map<char, Character> Characters;
	};

}

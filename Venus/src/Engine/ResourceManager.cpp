#include "pch.h"
#include "ResourceManager.h"

namespace Venus {

	std::unordered_map<std::string, Shader> ResourceManager::ShaderLibrary;
	std::unordered_map<std::string, Texture> ResourceManager::TextureLibrary;
	std::map<char, Character> ResourceManager::Characters;

    Shader ResourceManager::LoadShader(const std::string& vertex, const std::string& fragment, const std::string& name)
    {
        Shader shader(vertex, fragment);
        ShaderLibrary[name] = shader;

        return shader;
    }

    Shader& ResourceManager::GetShader(const std::string& name)
    {
        return ShaderLibrary[name];
    }

    Texture ResourceManager::LoadTexture(const std::string& fileName, const std::string& texName)
    {
        Texture texture(fileName, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR);
        TextureLibrary[texName] = texture;

        return texture;
    }

    Texture& ResourceManager::GetTexture(const std::string& name)
    {
        return TextureLibrary[name];
    }

    void ResourceManager::LoadFont(const std::string& font, unsigned int fontSize)
    {
        Characters.clear();

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            CORE_LOG_ERROR("Failed to initialize FreeType Library");
            return;
        }

        FT_Face face;
        if (FT_New_Face(ft, font.c_str(), 0, &face))
        {
            CORE_LOG_ERROR("Failed to Load Font: {0}", font);
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        // TODO: MAYBE MOVE TO TEXTURE
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (GLubyte c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                CORE_LOG_ERROR("Failed to Load Glyph: {0} : {1}", c, font);
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };

            Characters.insert(std::pair<char, Character>(c, character));
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    std::map<char, Character>& ResourceManager::GetFont()
    {
        return Characters;
    }

    void ResourceManager::Clear()
    {
        CORE_LOG_WARN("Cleaning Resources...");

        for (auto& element : ShaderLibrary)
            element.second.Delete();

        for (auto& element : TextureLibrary)
            element.second.Delete();
    }

}
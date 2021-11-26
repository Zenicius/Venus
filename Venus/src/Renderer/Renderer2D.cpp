#include "pch.h"
#include "Renderer2D.h"

namespace Venus {

    Renderer2D::Renderer2D(Shader& shader, Shader& textShader)
    {
        m_Shader = shader;
        m_TextShader = textShader;

        InitRender();
        InitRenderText();
    }

    Renderer2D::~Renderer2D()
    {
        m_VAO.Delete();
        m_TextVAO.Delete();
        m_TextVBO.Delete();
    }

    void Renderer2D::DrawSprite(Texture& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
    {
        m_Shader.Activate();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5 * size.y, 0.0f));
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        model = glm::scale(model, glm::vec3(size, 1.0f));

        //TODO: Find better way to use textures
        m_Shader.SetUniform1i("image", 0);
        m_Shader.SetUniformMat4("model", model);
        m_Shader.SetUniform3f("spriteColor", color);

        glActiveTexture(GL_TEXTURE0);
        texture.Bind();

        m_VAO.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_VAO.Unbind();

    }

    void Renderer2D::DrawText(const std::string& text, float x, float y, float scale, glm::vec3 color)
    {
        /*
        m_TextShader.Activate();
        m_TextShader.SetUniform3f("textColor", color);

        glActiveTexture(GL_TEXTURE0);
        m_TextVAO.Bind();

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = ResourceManager::GetFont()[*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y + (ResourceManager::GetFont()['H'].Bearing.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f },
                { xpos,     ypos,       0.0f, 0.0f },

                { xpos,     ypos + h,   0.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.Texture);

            m_TextVBO.Bind();
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            m_TextVBO.Unbind();

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale;
        }

        m_TextVAO.Unbind();
        glBindTexture(GL_TEXTURE_2D, 0);
        */
    }

    void Renderer2D::InitRender()
    {
        float vertices[] = {
            // pos      // tex
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

        VBO VBO(vertices, sizeof(vertices), GL_STATIC_DRAW);

        m_VAO.Bind();
        m_VAO.LinkAttrib(VBO, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);
        VBO.Unbind();
        m_VAO.Unbind();
    }

    void Renderer2D::InitRenderText()
    {
        m_TextShader.SetUniform1i("text", 0);

        m_TextVBO = VBO(NULL, sizeof(float) * 6 * 4, GL_DYNAMIC_DRAW);
        m_TextVAO.Bind();
        m_TextVAO.LinkAttrib(m_TextVBO, 0, 4, GL_FLOAT, 4 * sizeof(float), 0);
        m_TextVBO.Unbind();
        m_TextVAO.Unbind();
    }
}
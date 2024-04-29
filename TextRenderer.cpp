
#include "TextRenderer.h"

// CODE MOSTLY TAKEN FROM https://learnopengl.com/In-Practice/Text-Rendering

bool TextRenderer::load(std::string fontPath, unsigned int fontSize) {
    // Loading text
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return false;
    }
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return false;
    }

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; c++) {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void TextRenderer::renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
    // activate corresponding render state
    this->shader.use();
    glUniform3f(glGetUniformLocation(this->shader.ID, "textColor"), color.x, color.y, color.z);
    glBindVertexArray(VAO);

    // Attempt at centering text on screen
//    float totalWidth = 0.0f;
//    float maxHeight = 0.0f;
//    for (c = text.begin(); c != text.end(); c++) {
//        Character ch = Characters[*c];
//
//        std::cout << *c << std::endl;
//        std::cout << ch.Size.x << std::endl;
//        std::cout << ch.Size.y << std::endl;
//        std::cout << ch.Advance << std::endl;
//
//        totalWidth += (float) ch.Size.x * scale;
//        totalWidth += (float) (ch.Advance >> 6) * scale;
//        if ((float) ch.Size.y > maxHeight) {
//            maxHeight = (float) ch.Size.y;
//        }
//    }
//
//    totalWidth -= (float) (Characters[*(text.end() - 1)].Advance >> 6);
//
//    float distanceToMidWidth = totalWidth / 2.0f;
//    float distanceToMidHeight = maxHeight / 2.0f;
//
//    std::cout << "PRINT DOS VALORES FINAIS!!!!!" << std::endl;
//    std::cout << distanceToMidWidth << std::endl;
//    std::cout << distanceToMidHeight << std::endl;
//
//    x -= distanceToMidWidth;
//    y -= distanceToMidHeight;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + (float) ch.Bearing.x * scale;
        float ypos = y - (float) (ch.Size.y - ch.Bearing.y) * scale;

        float w = (float) ch.Size.x * scale;
        float h = (float) ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (float) (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
}

TextRenderer::TextRenderer(Shader &shader_) : shader(shader_) {}

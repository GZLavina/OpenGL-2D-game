
#ifndef LEARNOPENGL_TEXTRENDERER_H
#define LEARNOPENGL_TEXTRENDERER_H

#include "glad/glad.h"
#include "glm/vec3.hpp"
#include "shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H

class TextRenderer {
public:
    explicit TextRenderer(Shader &shader_);
    ~TextRenderer();
    bool load(std::string font, unsigned int fontSize);
    void renderText(std::string text, float x, float y, float scale, glm::vec3 color);
private:
    GLuint VAO;
    GLuint VBO;
    Shader &shader;
    FT_Library ft;
    FT_Face face;
    struct Character {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };
    std::map<char, Character> Characters; // Estrutura para armazenar cada caracter unico a ser renderizado
};


#endif //LEARNOPENGL_TEXTRENDERER_H

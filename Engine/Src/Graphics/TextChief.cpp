#include "TextChief.h"

extern int winWidth;
extern int winHeight;

TextChief::TextChief():
    ft(),
    face(),
    Characters({}),
    VAO(0),
    VBO(0)
{
}

TextChief::~TextChief(){
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

bool TextChief::Init(){
    if(FT_Init_FreeType(&ft)){
        puts("Failed to init FreeType!\n");
        return false;
    }
    if(FT_New_Face(ft, "Fonts/Yes.ttf", 0, &face)){
        puts("Failed to load font as face\n");
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 48); //Define pixel font size to extract from font face //0 for w so dynamically calc w based on h
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction

    for(unsigned char c = 0; c < 128; ++c){ //Load 1st 128 ASCII chars
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)){ //Load char glyph and set it as active
            printf("Failed to load \'%c\' as char glyph\n", char(c));
            continue;
        }

        uint texRefID;
        glGenTextures(1, &texRefID);
        glBindTexture(GL_TEXTURE_2D, texRefID);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Characters.insert(std::pair<char, Character>(c, {
            texRefID, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            uint(face->glyph->advance.x)
            }));

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0); 
    }

    return true;
}

void TextChief::RenderText(ShaderProg& SP, const str& text, const float& x, const float& y, const float& scaleFactor, const glm::vec3& colour){
    SP.Use();
    SP.Set3fv("textColour", colour);
    SP.SetMat4fv("projection", &glm::ortho(0.0f, (float)winWidth, 0.0f, (float)winHeight)[0][0]);

    //glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for(std::string::const_iterator c = text.begin(); c != text.end(); ++c){
        Character ch = Characters[*c];
        float xpos = x + ch.Bearing.x * scaleFactor;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scaleFactor;

        float w = ch.Size.x * scaleFactor;
        float h = ch.Size.y * scaleFactor;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };

        SP.UseTex(ch.texRefID, "texSampler");

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        const_cast<float&>(x) += (ch.Advance >> 6) * scaleFactor; // now advance cursors for next glyph (note that advance is number of 1/64 pixels) // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    SP.ResetTexUnits();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}
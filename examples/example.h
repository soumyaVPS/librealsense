#define GLFW_INCLUDE_GLU

#include <GLFW/glfw3.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "third_party/stb_truetype.h"

#include "stdio.h"

struct font
{
    GLuint tex;
    stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyph
};

static struct font ttf_create(FILE * f)
{
    int buffer_size;
    void * buffer;
    unsigned char temp_bitmap[512*512];
    struct font fn;

    fseek(f, 0, SEEK_END);
    buffer_size = ftell(f);

    buffer = malloc(buffer_size);
    fseek(f, 0, SEEK_SET);
    fread(buffer, 1, buffer_size, f);

    stbtt_BakeFontBitmap((unsigned char*)buffer,0, 20.0, temp_bitmap, 512,512, 32,96, fn.cdata); // no guarantee this fits!
    free(buffer);

    glGenTextures(1, &fn.tex);
    glBindTexture(GL_TEXTURE_2D, fn.tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return fn;
}

static void ttf_print(struct font * font, float x, float y, const char *text)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font->tex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    for(; *text; ++text)
    {
        if (*text >= 32 && *text < 128)
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->cdata, 512,512, *text-32, &x,&y,&q,1);
            glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y0);
            glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y0);
            glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y1);
            glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y1);
        }
    }
    glEnd();
    glPopAttrib();
}

static float ttf_len(struct font * font, const char *text)
{
    float x=0, y=0;
    for(; *text; ++text)
    {
        if (*text >= 32 && *text < 128)
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->cdata, 512,512, *text-32, &x,&y,&q,1);
        }
    }
    return x;
}
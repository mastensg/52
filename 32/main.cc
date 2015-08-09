#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "gl_core_3_3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "gl.h"
#include "res.h"

typedef struct
{
    GLuint program;
    GLuint texture;
    GLuint vao;
    GLuint vbo;
    GLint a_position;
    GLint a_texcoord;
    GLint u_texture;
    GLint u_textcolor;
    GLint u_viewport;
} TextProgram;

typedef struct
{
    float s;
    float t;
    float w;
    float h;
    float left;
    float top;
    float advance_x;
} Glyph;

////////////////////////////////////////////////////////////////////////

static void
glfw_error(int error, const char *description)
{
    fprintf(stderr, "GLFW error: %d %s", error, description);
}

static void
glfw_key(GLFWwindow *window, int key, int, int action, int)
{
    if (GLFW_PRESS != action && GLFW_REPEAT != action)
        return;

    if (GLFW_KEY_ESCAPE == key)
        glfwSetWindowShouldClose(window, 1);
}

static void
glfw_window_size(GLFWwindow *, int w, int h)
{
    glViewport(0, 0, w, h);
}

////////////////////////////////////////////////////////////////////////

static void
gl_debug(GLenum source, GLenum, GLuint id, GLenum, GLsizei,
         const GLchar *message, const void *)
{
    if (131185 == id)
        return;

    std::fprintf(stderr, "%d %d %s\n", source, id, message);
    // std::exit(1);
}

GLuint
compile_shader(GLenum type, const GLchar *source, GLint *size)
{
    GLuint shader;
    GLint infoLen;
    GLint compiled;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, size);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (1 < infoLen)
    {
        GLchar infoLog[4096];

        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::fprintf(stderr, "shader compilation:\n%s\n", infoLog);
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
        exit(EXIT_FAILURE);

    return shader;
}

GLuint
link_program(GLuint vs, GLuint fs)
{
    GLint infoLen;
    GLuint program;
    GLint linked;

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    if (1 < infoLen)
    {
        GLchar infoLog[4096];

        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        std::fprintf(stderr, "program linking:\n%s\n", infoLog);
    }

    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
        exit(EXIT_FAILURE);

    return program;
}

////////////////////////////////////////////////////////////////////////

Glyph glyphs[128];

// probably broken
uint32_t
text_width(const char *string)
{
    Glyph *g;
    size_t i;
    uint32_t w = 0;

    if (!strlen(string))
        return w;

    for (i = 0; i < strlen(string) - 1; ++i)
    {
        g = glyphs + string[i];
        w += g->advance_x;
    }

    g = glyphs + string[i];
    w += g->w;

    return w;
}

void
text_draw(GLfloat x, GLfloat y, const char *string)
{
    for (size_t i = 0; i < strlen(string); ++i)
    {
        const Glyph *g = glyphs + string[i];
        const GLfloat attrs[] = {
            x, y - g->top, g->s, g->t,                //
            x, y - g->top + g->h, g->s, g->t + g->h,  //
            x + g->w, y - g->top, g->s + g->w, g->t,  //
            x + g->w, y - g->top + g->h, g->s + g->w, g->t + g->h,
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(attrs), attrs, GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        x += g->advance_x;
    }
}

////////////////////////////////////////////////////////////////////////

int the_natural = 0;

static void *
update_thread(void *arg)
{
    for (;;)
    {
        if (EOF == scanf("%d\n", &the_natural))
            exit(EXIT_SUCCESS);
    }

    return arg;
}

int
main(int, const char *argv[])
{
    {
        pthread_attr_t attr;
        pthread_t update;

        pthread_attr_init(&attr);

        if (pthread_create(&update, &attr, update_thread, NULL))
            err(EXIT_FAILURE, "pthread_create");
    }

    const uint16_t font_size = 48;
    uint8_t font_atlas[1024 * 1024];

    // Render font atlas
    FT_Face face;
    {
        FT_Error error;
        FT_Library ftl;
        FT_UInt index;
        int i;
        uint16_t s, t, w, h;

        if ((error = FT_Init_FreeType(&ftl)))
            errx(EXIT_FAILURE, "FT_Init_FreeType: %d", error);

        if ((error = FT_New_Face(
                 ftl, "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf", 0,
                 &face)))
            errx(EXIT_FAILURE, "FT_NewFace: %d", error);

        if ((error = FT_Set_Pixel_Sizes(face, 0, font_size)))
            errx(EXIT_FAILURE, "FT_Set_Pixel_Sizes: %d", error);

        s = 0;
        t = 0;
        for (i = 0; i < 128; ++i)
        {
            index = FT_Get_Char_Index(face, i);

            if ((error = FT_Load_Glyph(face, index, FT_LOAD_DEFAULT)))
                errx(EXIT_FAILURE, "FT_Load_Glyph: %d", error);

            if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
            {
                if ((error =
                         FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)))
                    errx(EXIT_FAILURE, "FT_Render_Glyph: %d", error);
            }

            glyphs[i].left = face->glyph->bitmap_left;
            glyphs[i].top = face->glyph->bitmap_top;
            glyphs[i].advance_x = face->glyph->advance.x / 64.0;

            w = face->glyph->bitmap.width;
            h = face->glyph->bitmap.rows;

            glyphs[i].w = w;
            glyphs[i].h = h;

            if (1024 <= s + w)
            {
                s = 0;
                t += font_size;
            }

            glyphs[i].s = s;
            glyphs[i].t = t;

            unsigned x, y;
            for (y = 0; y < h; ++y)
            {
                for (x = 0; x < w; ++x)
                {
                    uint8_t p = face->glyph->bitmap.buffer[y * w + x];

                    font_atlas[1024 * (t + y) + s + x] = p;
                }
            }

            s += w;
        }
    }

    // GLFW init
    GLFWwindow *theWindow;
    glfwSetErrorCallback(glfw_error);

    if (!glfwInit())
        errx(EXIT_FAILURE, "cannot initialize glfw");

    if (!(theWindow = glfwCreateWindow(1024, 1024, argv[0], NULL, NULL)))
    {
        glfwTerminate();
        errx(EXIT_FAILURE, "cannot create window");
    }

    glfwMakeContextCurrent(theWindow);

    glfwSetKeyCallback(theWindow, glfw_key);
    glfwSetWindowSizeCallback(theWindow, glfw_window_size);

    // OpenGL init
    if (ogl_LOAD_FAILED == ogl_LoadFunctions())
    {
        glfwTerminate();
        errx(EXIT_FAILURE, "cannot load opengl functions");
    }

    glDebugMessageCallback(gl_debug, NULL);
    glEnable(GL_DEBUG_OUTPUT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    TextProgram tp;
    {
        GLuint vs, fs;

        vs = compile_shader(GL_VERTEX_SHADER, res_text_v, &res_text_v_size);
        fs = compile_shader(GL_FRAGMENT_SHADER, res_text_f, &res_text_f_size);
        tp.program = link_program(vs, fs);

        tp.a_position = glGetAttribLocation(tp.program, "position");
        tp.a_texcoord = glGetAttribLocation(tp.program, "texcoord");

        tp.u_textcolor = glGetUniformLocation(tp.program, "textcolor");
        tp.u_texture = glGetUniformLocation(tp.program, "texture1");
        tp.u_viewport = glGetUniformLocation(tp.program, "viewport");

        glGenBuffers(1, &tp.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, tp.vbo);

        glGenVertexArrays(1, &tp.vao);
        glBindVertexArray(tp.vao);
        {
            GLsizei stride = 4 * sizeof(GLfloat);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                                  (GLvoid *)(2 * sizeof(GLfloat)));
        }
        glEnableVertexAttribArray(tp.a_position);
        glEnableVertexAttribArray(tp.a_texcoord);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tp.texture);
        glBindTexture(GL_TEXTURE_RECTANGLE, tp.texture);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, 1024, 1024, 0, GL_RED,
                     GL_UNSIGNED_BYTE, font_atlas);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST);
    }

    while (!glfwWindowShouldClose(theWindow))
    {
        GLint viewport[4];
        char string[256];

        glGetIntegerv(GL_VIEWPORT, viewport);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(tp.program);
        glUniform2f(tp.u_viewport, viewport[2], viewport[3]);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(tp.u_texture, 0);
        glUniform3f(tp.u_textcolor, 1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_RECTANGLE, tp.texture);

        glBindBuffer(GL_ARRAY_BUFFER, tp.vbo);
        glBindVertexArray(tp.vao);

        std::sprintf(string, "%d", the_natural);

        text_draw(100, 400, string);

        glfwSwapBuffers(theWindow);
        glfwPollEvents();
    }

    glfwTerminate();
}

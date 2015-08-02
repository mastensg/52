#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <err.h>

#include "gl_core_3_3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <magick/api.h>

#include "res.h"

typedef struct
{
    GLuint program;
    GLuint texture;
    GLuint vao;
    GLuint vbo;
    GLint a_position;
    GLint u_texture;
} ImageProgram;

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

#ifdef DEBUG
static void
gl_debug(GLenum source, GLenum, GLuint id, GLenum, GLsizei,
         const GLchar *message, const void *)
{
    if (131185 == id)
        return;

    std::fprintf(stderr, "%d %d %s\n", source, id, message);
}
#endif

GLuint
compile_shader(GLenum type, const GLchar *source, GLint *size)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, size);
    glCompileShader(shader);

#ifdef DEBUG
    GLint infoLen;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (1 < infoLen)
    {
        GLchar infoLog[4096];

        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::fprintf(stderr, "shader compilation:\n%s\n", infoLog);
    }
#endif

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
        errx(EXIT_FAILURE, "cannot compile shader");

    return shader;
}

GLuint
link_program(GLuint vs, GLuint fs)
{
    GLuint program;
    GLint linked;

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

#ifdef DEBUG
    GLint infoLen;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
    if (1 < infoLen)
    {
        GLchar infoLog[4096];

        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        std::fprintf(stderr, "program linking:\n%s\n", infoLog);
    }
#endif

    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
        errx(EXIT_FAILURE, "cannot link program");

    return program;
}

int
main(int argc, const char *argv[])
{
    if (2 != argc)
        errx(EXIT_FAILURE, "usage: %s image", argv[0]);

    // GLFW init
    GLFWwindow *window;
    glfwSetErrorCallback(glfw_error);

    if (!glfwInit())
        errx(EXIT_FAILURE, "cannot initialize glfw");

    if (!(window = glfwCreateWindow(1024, 1024, argv[0], NULL, NULL)))
    {
        glfwTerminate();
        errx(EXIT_FAILURE, "cannot create window");
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, glfw_key);
    glfwSetWindowSizeCallback(window, glfw_window_size);

    // OpenGL init
    if (ogl_LOAD_FAILED == ogl_LoadFunctions())
    {
        glfwTerminate();
        errx(EXIT_FAILURE, "cannot load opengl functions");
    }

#ifdef DEBUG
    glDebugMessageCallback(gl_debug, NULL);
    glEnable(GL_DEBUG_OUTPUT);
    std::fprintf(stderr, "DEBUG enabled\n");
#endif

    // Image
    ImageProgram ip;
    {
        GLuint vs, fs;

        vs = compile_shader(GL_VERTEX_SHADER, res_image_v, &res_image_v_size);
        fs = compile_shader(GL_FRAGMENT_SHADER, res_image_f, &res_image_f_size);
        ip.program = link_program(vs, fs);

        ip.a_position = glGetAttribLocation(ip.program, "position");

        ip.u_texture = glGetUniformLocation(ip.program, "texture1");

        glGenBuffers(1, &ip.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, ip.vbo);

        glGenVertexArrays(1, &ip.vao);
        glBindVertexArray(ip.vao);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(ip.a_position);

        uint8_t img[256 * 256 * 3];

        for (int y = 0; y < 256; ++y)
        {
            for (int x = 0; x < 256; ++x)
            {
                img[256 * 3 * y + 3 * x + 0] = x;
                img[256 * 3 * y + 3 * x + 1] = y;
                img[256 * 3 * y + 3 * x + 2] = x ^ y;
            }
        }

        Image *image;
        ImageInfo *imageInfo;
        ExceptionInfo exceptionInfo;

        InitializeMagick(NULL);

        imageInfo = CloneImageInfo(0);
        GetExceptionInfo(&exceptionInfo);

        strcpy(imageInfo->filename, argv[1]);

        if (!(image = ReadImage(imageInfo, &exceptionInfo)))
        {
            CatchException(&exceptionInfo);
            return EXIT_FAILURE;
        }

        std::fprintf(stderr, "%dx%d\n", image->columns, image->rows);

        PixelPacket *pixels;

        pixels = GetImagePixels(image, 0, 0, image->columns, image->rows);
        if (!pixels)
            errx(EXIT_FAILURE, "cannot get pixels");

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &ip.texture);
        glBindTexture(GL_TEXTURE_2D, ip.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->columns, image->rows, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.3f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ip.program);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(ip.u_texture, 0);
        glBindTexture(GL_TEXTURE_2D, ip.texture);

        glBindVertexArray(ip.vao);
        glBindBuffer(GL_ARRAY_BUFFER, ip.vbo);

        const GLfloat one = 1.0;
        const GLfloat attrs[] = {
            -one, +one,  //
            -one, -one,  //
            +one, +one,  //
            +one, -one,  //
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(attrs), attrs, GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

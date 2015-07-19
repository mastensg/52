#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>

#include <err.h>

#include "gl_core_3_3.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "res.h"

typedef struct
{
    GLuint program;
    GLuint vao;
    GLuint vbo;
    GLint a_position;
} DotProgram;

typedef struct
{
    float x, y, dx, dy, ox, oy;
    int frames;
} Dot;

static const double tau = 6.28318530718;

static const float kDotSpeed = 0.02;
static const uint64_t kNumDots = 64;

static void
dot_init(Dot *dot)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> idis(0, 30 * 60);
    std::uniform_real_distribution<> adis(0.0, tau);
    std::uniform_real_distribution<> ndis(-1.0, 1.0);

    double a = adis(gen);

    dot->x = 6.0 * cos(a);
    dot->y = 6.0 * sin(a);
    dot->dx = -kDotSpeed * cos(a + 0.1 * ndis(gen));
    dot->dy = -kDotSpeed * sin(a + 0.1 * ndis(gen));
    dot->ox = ndis(gen);
    dot->oy = ndis(gen);
    dot->frames = idis(gen);
}

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

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
        exit(EXIT_FAILURE);

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

    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
        exit(EXIT_FAILURE);

    return program;
}

int
main(int, const char *argv[])
{
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
#endif

    // Dots
    DotProgram dp;
    {
        GLuint vs, fs;

        vs = compile_shader(GL_VERTEX_SHADER, res_dot_v, &res_dot_v_size);
        fs = compile_shader(GL_FRAGMENT_SHADER, res_dot_f, &res_dot_f_size);
        dp.program = link_program(vs, fs);

        dp.a_position = glGetAttribLocation(dp.program, "position");

        glGenBuffers(1, &dp.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, dp.vbo);

        glGenVertexArrays(1, &dp.vao);
        glBindVertexArray(dp.vao);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(dp.a_position);
    }

    Dot dots[kNumDots];
    for (uint64_t d = 0; d < kNumDots; ++d)
    {
        dot_init(dots + d);
    }

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(dp.program);

        glBindVertexArray(dp.vao);
        glBindBuffer(GL_ARRAY_BUFFER, dp.vbo);

        for (uint64_t d = 0; d < kNumDots; ++d)
        {
            Dot *dot = dots + d;

            if (dot->frames)
            {
                --dot->frames;
                continue;
            }

            float ca, cb, za, zb;
            GLfloat attrs[2 * 256];

            ca = dot->x;
            cb = dot->y;
            za = ca;
            zb = cb;

            for (int i = 0; i < 256; ++i)
            {
                float zaa = za * za;
                float zbb = zb * zb;

                attrs[2 * i + 0] = dot->ox + 0.5 * za;
                attrs[2 * i + 1] = dot->oy + 0.5 * zb;

                zb = 2.0 * za * zb + cb;
                za = zaa - zbb + ca;
            }

            attrs[0] = 10.0;
            attrs[1] = 10.0;

            glBufferData(GL_ARRAY_BUFFER, sizeof(attrs), attrs, GL_STREAM_DRAW);
            glDrawArrays(GL_POINTS, 0, 256);

            dot->x += dot->dx;
            dot->y += dot->dy;

            if (100.0 < dot->x * dot->x + dot->y * dot->y)
                dot_init(dot);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

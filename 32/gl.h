const char *gl_strerror(GLint error);

GLuint compile_shader(GLenum type, const GLchar *source, GLint *size);

GLuint link_program(GLuint vs, GLuint fs);

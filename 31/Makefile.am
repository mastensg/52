AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++14

bin_PROGRAMS = form

form_CXXFLAGS = $(GLFW_CFLAGS) $(GM_CFLAGS)
form_LDADD = $(GLFW_LIBS) $(GM_LIBS) -lGL
form_SOURCES = main.cc \
		   gl_core_3_3.c gl_core_3_3.h \
		   res.s res.h \
		   image.v.glsl image.f.glsl

res.o: image.v.glsl image.f.glsl

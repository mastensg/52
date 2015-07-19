AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++14

bin_PROGRAMS = dotchamber

dotchamber_CFLAGS = $(GLFW_CFLAGS)
dotchamber_LDADD = $(GLFW_LIBS) -lGL -lm
dotchamber_SOURCES = main.cc \
		   gl_core_3_3.c gl_core_3_3.h \
		   res.s res.h \
		   dot.v.glsl dot.f.glsl

res.o: dot.v.glsl dot.f.glsl

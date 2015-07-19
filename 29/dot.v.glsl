#version 130

in vec2 position;

void main(void)
{
    gl_Position = vec4(vec2(0.5) * position, 0.0, 1.0);
}

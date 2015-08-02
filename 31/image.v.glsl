#version 130

in vec2 position;

out vec2 texpos;

void main(void)
{
    gl_Position = vec4(position, 0.0, 1.0);

    texpos = 0.5 * vec2(position.x, -position.y) - vec2(0.5);
}

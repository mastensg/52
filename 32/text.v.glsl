#version 130

in vec2 position;
in vec2 texcoord;

out vec2 UV;

uniform vec2 viewport;

void main(void)
{
    vec2 pos;

    pos.x = ( position.x - 0.5 * viewport.x) * (2.0 / viewport.x);
    pos.y = (-position.y + 0.5 * viewport.y) * (2.0 / viewport.y);

    gl_Position = vec4(pos, 0.0, 1.0);

    UV = texcoord * vec2(1.0/1024.0, 1.0/1024.0);
    UV = texcoord;
}

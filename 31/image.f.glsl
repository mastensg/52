#version 130

in vec2 texpos;

out vec3 color;

uniform sampler2D texture1;

void main(void)
{
    color = texture(texture1, texpos).rgb;
}

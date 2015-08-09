#version 130

in vec2 UV;

out vec4 color;

uniform sampler2DRect texture1;
uniform vec3 textcolor;

void main(void)
{
    color = vec4(textcolor, texture(texture1, UV).r);
}

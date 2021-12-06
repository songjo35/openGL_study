#version 150 core

out vec4 out_Color;

in vec2 texCoord;
in vec3 finalColor;


void main(void)
{
	out_Color = vec4(finalColor, 1.0);
}

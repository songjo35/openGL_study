#version 150 core

out vec4 out_Color;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 lightInt;
uniform vec3 ambInt;
uniform vec3 color;
uniform vec3 cameraPos;
uniform int shininess = 20;

// directional light
const vec3 L = normalize(vec3(3, 5, 5));


void main(void)
{
	vec3 N = normalize(normal);
	vec3 V = normalize(cameraPos - worldPos);
	vec3 H = normalize(L+V);
	
	vec3 ambient = color * ambInt;
	vec3 diffuse = lightInt * max(0,dot(L,N)) * color;
	vec3 specular = lightInt * pow(max(0,dot(N, H)), shininess);
	vec3 I =  ambient + diffuse +specular;
	//vec3 I = ka*ambient + lightInt * (kd*max(0,dot(L,N))*color + ks*pow(max(0,dot(N, H)), shininess));
	out_Color = vec4(I, 1.0);
}

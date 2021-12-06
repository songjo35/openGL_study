#version 410 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoord;

uniform mat4 modelMat = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 viewMat = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 projMat = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

uniform vec3 lightPos;
uniform vec3 lightInt;
uniform vec3 ambInt;
uniform vec3 color;
uniform vec3 cameraPos;

out vec3 worldPos;
out vec3 normal;
out vec2 texCoord;
out vec3 finalColor;

int shininess = 20;

void main(void)
{
	// 노멀벡터는 translate 할수없음 동차좌표에서 0을 넣으면 됨
	worldPos = (modelMat * vec4(in_Position, 1)).xyz;
	normal = (modelMat * vec4(in_Normal, 0)).xyz;
	texCoord = in_TexCoord;

	vec3 p3 = in_Position.xyz;
	vec4 p = projMat * viewMat * modelMat * vec4(p3, 1);
	gl_Position = p;

	// phong lighting model
	vec3 L = normalize(lightPos -worldPos);
	vec3 N = normalize(normal);
	vec3 V = normalize(cameraPos - worldPos);
	vec3 R = 2*dot(N,L)*N-L;

	vec3 ambient = color * ambInt;
	vec3 diffuse = lightInt * max(0,dot(N,L)) * color;
	vec3 specular = lightInt * max(0,dot(N,L)) * pow(max(0,dot(R,V)), shininess);
	vec3 I = ambient + diffuse + specular;
	finalColor = I;
}

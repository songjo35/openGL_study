#version 150 core

out vec4 out_Color;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 lightPos;
uniform vec3 lightInt;
// intensity
uniform vec3 ambInt;
uniform vec3 diffColor;
uniform vec3 specColor;
uniform vec3 cameraPos;
uniform int shininess = 20;
uniform float refractive_index;

uniform float ka = 1.0;
uniform float kd = 1.0;
uniform float ks = 1.0;

void main(void)
{
	vec3 L = normalize(lightPos -worldPos);
	// 선형보간된 n은 길이가 짧아짐
	vec3 N = normalize(normal);
	vec3 V = normalize(cameraPos - worldPos); // normalize(-worldPos);
	vec3 R = 2*dot(N,L)*N-L; // reflect(-L, N);

	// Lambert's cosine law
	float lambertian = max(0, dot(N,L));


	vec3 ambient = diffColor * ambInt;
	vec3 diffuse = lightInt * lambertian * diffColor;
	//vec3 dielectric = specColor * refractive_index;
	vec3 specular = lightInt * lambertian * pow(max(0,dot(R,V)), shininess) * specColor; // dielectric;
	vec3 I = ka*ambient + kd*diffuse + ks*specular;
	//I = ka*diffColor*ambInt + lightInt*lambertian * (kd*diffColor + ks*pow(max(0,dot(R,V)),shininess)*specColor);

	out_Color = vec4(I, 1.0);
	// srgb match
	// out_Color = vec4(pow(I, vec3(1/2.2)), 1.0);
}

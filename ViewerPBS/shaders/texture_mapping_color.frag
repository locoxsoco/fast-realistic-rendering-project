#version 330

in vec2 TexCoords;
smooth in vec3 eye_normal;
smooth in vec3 eye_vertex;

uniform vec3 light_position;
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;
uniform sampler2D texture_color;

vec3 light_color = vec3(1,1,1);

layout (location = 0) out vec4 frag_lightning;
layout (location = 1) out vec4 frag_color;

void main (void) {
 // Light vector
 vec3 L = normalize(light_position-eye_vertex);
 // Normal vector
 vec3 N = normalize(eye_normal);
 // Eye view vector
 vec3 V = normalize(-eye_vertex);
 // Light Reflection vector
 vec3 R_L = normalize(reflect(-L, N));

 // Calculate Ambient Term:
 float ambient_strength = 0.5;
 vec3 Iamb = light_color;
 Iamb = Iamb * ambient_strength;

 // Calculate Diffuse Term:
 vec3 Idiff = vec3(max(dot(N, L), 0.0)) * light_color;
 Idiff = Idiff * material_diffuse;

 // Calculate Specular Term:
 vec3 Ispec = pow(max(dot(R_L, V), 0.0), material_shininess) * light_color;
 Ispec = Ispec * material_specular;

 // Write Total Color:
 frag_lightning = vec4(Iamb + Idiff + Ispec , 1.0);
 frag_color = texture(texture_color, TexCoords);
}

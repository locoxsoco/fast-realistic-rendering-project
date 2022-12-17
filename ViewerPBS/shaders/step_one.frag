#version 330

smooth in vec3 eye_normal;
smooth in vec3 eye_vertex;

uniform vec3 light_position;
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

vec3 light_color = vec3(1,1,1);

//layout (location = 0) out vec4 frag_color;
layout (location = 0) out vec4 frag_normal;

void main (void) {
 vec3 N = normalize(eye_normal);

 //vec3 albedo = vec3(0.3125f, 0.859375f, 0.390625f);

 // write Total Color:
 //frag_color = vec4(albedo, 1.0);

 frag_normal = vec4(N, 1.0);
}

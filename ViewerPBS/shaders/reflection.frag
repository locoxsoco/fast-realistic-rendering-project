#version 330

smooth in vec3 eye_normal;
smooth in vec3 eye_vertex;

uniform samplerCube texture_chosen;
uniform mat4 inverse_view;

layout (location = 0) out vec4 frag_lightning;
layout (location = 1) out vec4 frag_color;

void main (void) {
    vec3 V = eye_vertex;
    vec3 N = normalize(eye_normal);
    vec3 R_V = vec3(inverse_view * vec4(reflect(V, N),1.0));
    frag_color = vec4(texture(texture_chosen, R_V).rgb, 1.0);
    frag_lightning = vec4(1.f,1.f,1.f,1.f);
}

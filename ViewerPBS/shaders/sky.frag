#version 330

smooth in vec3 world_vertex;

uniform samplerCube skybox_map_;

out vec4 frag_color;

void main (void) {
 vec3 V = normalize(world_vertex);
 frag_color = texture(skybox_map_, V);
}

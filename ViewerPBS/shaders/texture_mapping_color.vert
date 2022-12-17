#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coords;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normal_matrix;

smooth out vec3 eye_normal;
smooth out vec3 eye_vertex;
out vec2 TexCoords;

void main(void)  {
  TexCoords = vec2(texture_coords.x,texture_coords.y);
  vec4 view_vertex = view * model * vec4(vertex, 1);
  eye_vertex = view_vertex.xyz;
  eye_normal = normalize(normal_matrix * normal);

  gl_Position = projection * view_vertex;
}

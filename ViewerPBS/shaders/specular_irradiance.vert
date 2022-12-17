#version 330
layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 view;

smooth out vec3 world_vertex;

void main(void)  {
  world_vertex = vertex;
  gl_Position = projection * view * vec4(vertex, 1);
}

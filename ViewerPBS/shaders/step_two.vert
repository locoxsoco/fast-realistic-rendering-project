#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 2) in vec2 texture_coords;

out vec2 TexCoords;
out vec2 Vertex;

void main()
{
    TexCoords = texture_coords;
    Vertex = vertex/2.f+0.5f;
    gl_Position = vec4(vertex.x, vertex.y, 0.0, 1.0);
}

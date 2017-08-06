#version 150

// in-data for vertex shader comes from the vertex buffer
in vec2 position;
in vec3 color;

// out-data for vertex shader goes to fragment shader
out vec3 Color;

void main()
{
    Color = color;
    gl_Position = vec4(position.x, -position.y, 0.0, 1.0);
}

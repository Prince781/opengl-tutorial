#version 150

// in-data for vertex shader comes from the vertex buffer
in vec2 position;
in vec3 color; // vertex color (not texture color)
in vec2 texcoord; // (s,t)

// out-data for vertex shader goes to the fragment shader
out vec3 Color;
out vec2 Texcoord;

// matrix transformation
uniform mat4 trans;

void main()
{
    Color = color;
    Texcoord = texcoord;
    gl_Position = vec4(position, 0.0, 1.0);
}

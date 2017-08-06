#version 150

// in-data for vertex shader comes from the vertex buffer
in vec2 position;
in vec3 color; // vertex color (not texture color)
in vec2 texcoord; // (s,t)

// out-data for vertex shader goes to the fragment shader
out vec3 Color;
out vec2 Texcoord;

// transforms a model position to a position in the world
uniform mat4 model;
// transforms the world according to camera movement
uniform mat4 view;
// projection transform (for perspective)
uniform mat4 proj;

void main()
{
    Color = color;
    Texcoord = texcoord;
    gl_Position = proj * view * model * vec4(position, 0.0, 1.0);
}

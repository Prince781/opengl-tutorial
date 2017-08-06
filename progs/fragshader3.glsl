#version 150 core

// in-data for fragment shader comes from vertex shader
in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

// allows us to sample texture data; contains texture color
// uniforms are automatic global variables that can be
// accessed by library functions
uniform sampler2D tex;

void main()
{
    /* the function texture(tex: Texture, coord: vec2)
     * allows us to sample a color from a texture
     */
    outColor = texture(tex, Texcoord) * vec4(Color, 1.0);
}

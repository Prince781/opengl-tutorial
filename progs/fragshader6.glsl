#version 150

// in-data for fragment shader comes from vertex shader
in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

// allows us to sample texture data; contains texture color
// sampler uniforms come from a texture unit
uniform sampler2D texKitten;
uniform sampler2D texPuppy;

// to attenuate the reflection
uniform vec3 overrideColor;

void main()
{
    vec4 texColor = mix(texture(texKitten, Texcoord),
                        texture(texPuppy, Texcoord), 0.5);
    outColor = vec4(overrideColor * Color, 1.0) * texColor;
}


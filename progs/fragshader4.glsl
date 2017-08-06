#version 150 core

#define M_PI 3.1415926535897932384626433832795

// in-data for fragment shader comes from vertex shader
in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;

// allows us to sample texture data; contains texture color
// sampler uniforms come from a texture unit
uniform sampler2D texKitten;
uniform sampler2D texPuppy;

uniform float time;

void main()
{
    /* the function texture(tex: Texture, coord: vec2)
     * allows us to sample a color from a texture
     */
    float rLevel = 0.5;  // reflection level
    vec4 colKitten;
    vec4 colPuppy;
    if (Texcoord.y < rLevel) {
        colKitten = texture(texKitten, Texcoord);
        colPuppy = texture(texPuppy, Texcoord);
    } else {
        float dy = Texcoord.y - rLevel;
        float speed = 0.3;
        float distort = 20;
        float x = Texcoord.x + 0.4*dy/(1-rLevel) * 
            (1.0 + sin(time*speed + dy*distort))/2.0;
        colKitten = texture(texKitten, vec2(x, 2*rLevel - Texcoord.y));
        colPuppy = texture(texPuppy, vec2(x, 2*rLevel -Texcoord.y));
    }
    /* mix() linearly interpolates between two variables,
     * based on some constant c between 0 and 1 */
    outColor = mix(colKitten, colPuppy, (1.0 + sin(time*M_PI/3))/2.0);
}

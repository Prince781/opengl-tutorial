#version 150

// in-data for fragment shader comes from the vertex shader
in vec3 Color;

// out-data for fragment shader goes to the color buffer
out vec4 outColor;

void main()
{
    // outColor = vec4(1-Color.r,1-Color.g,1-Color.b, 1.0);
    float avg = (Color.r + Color.g + Color.b)/ 3;
    outColor = vec4(avg, avg, avg, 1.0);
}

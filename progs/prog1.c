#include "../util.h"
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

static GLuint vertexBuffer;
static float vertices[] = {
    0.0f, 0.5f, // vertex 1 (x,y)
    0.5f, -0.5f,
    -0.5f, -0.5f
};

static GLuint vertexShader, fragmentShader;

static GLuint vao;
static GLuint shaderProgram;

static struct timespec tm_spec;

void prog1_init(void)
{
    printf("OpenGL version is %s\n", glGetString(GL_VERSION));

    /* VAOs store links between attributes in a program and raw data.
     * This shuold come before any calls to glVertexAttribPointer */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* generate VBO */
    glGenBuffers(1, &vertexBuffer);
    printf("vbo = %u\n", vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint status;
    char err_buf[512];

    int num_lines;
    char **vsh_lines;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    vsh_lines = get_lines("progs/vshader1.glsl", &num_lines);
    glShaderSource(vertexShader, num_lines, (const GLchar **) vsh_lines, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling vshader1.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader vshader1.glsl\n");

    char **fsh_lines;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fsh_lines = get_lines("progs/fragshader1.glsl", &num_lines);
    glShaderSource(fragmentShader, num_lines, (const GLchar **) fsh_lines, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling fragshader1.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader fragshader1.glsl\n");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error linking program:\n%s\n", err_buf);
    } else
        printf("Linked shader program.\n");
    glUseProgram(shaderProgram);

    /* for the vertex shader */
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    /* see https://open.gl/drawing
     * this attaches the buffer we are currently using (see glBindBuffer)
     * to the "position" object in the vertex shader */
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

    /* initialize clock */
    clock_gettime(CLOCK_REALTIME, &tm_spec);
}

void prog1_uninit(void)
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
}

void prog1_render(void)
{
    struct timespec tmp;
    
    clock_gettime(CLOCK_REALTIME, &tmp);

    double diff_sec = difftime(tmp.tv_sec, tm_spec.tv_sec);
    diff_sec += 1e-9 * (tmp.tv_nsec - tm_spec.tv_nsec);
    double red = sin(diff_sec * M_PI/3);
    double blue = cos(diff_sec * M_PI/3);
    double green = red * blue;
    GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
    glUniform3f(uniColor,
            red,
            blue,
            green);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* the VAO is already bound
     * draws the triangle from the 3 vertices */
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glFlush();
}

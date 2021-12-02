#include "../util.h"
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>

static GLuint vertexBuffer;
static float vertices[] = {
    -0.4f, 0.4f, 1.0f, 0.0f, 0.0f, // top-left
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // top-right
    0.4f, -0.4f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, .5f, .5f, 0.0f, // bottom-left
};

static GLuint elementBuffer;
/* reuse vertices from the vertex buffer to draw two triangles */
static GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
};

static GLuint vertexShader, fragmentShader;

static GLuint vao;
static GLuint shaderProgram;

void prog2_init(void)
{
    printf("OpenGL version is %s\n", glGetString(GL_VERSION));
    printf("GLSL version is %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("OpenGL renderer is %s\n", glGetString(GL_RENDERER));
    printf("Demonstration of colors in vertex shader, and element buffers.\n");

    /* VAOs store links between attributes in a program and raw data.
     * This shuold come before any calls to glVertexAttribPointer */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* generate VBO */
    glGenBuffers(1, &vertexBuffer);
    printf("vbo = %u\n", vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &elementBuffer);
    printf("ebo = %u\n", elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    GLint status;
    char err_buf[512];

    int num_lines;
    char **vsh_lines;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    vsh_lines = get_lines("progs/vshader2.glsl", &num_lines);
    glShaderSource(vertexShader, num_lines, (const GLchar **) vsh_lines, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling vshader2.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader vshader2.glsl\n");

    char **fsh_lines;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fsh_lines = get_lines("progs/fragshader2.glsl", &num_lines);
    glShaderSource(fragmentShader, num_lines, (const GLchar **) fsh_lines, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling fragshader2.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader fragshader2.glsl\n");

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
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);

    /* for the vertex shader */
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float),
            (void*)(2*sizeof(float)));
}

void prog2_uninit(void)
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteVertexArrays(1, &vao);
}

void prog2_render(void)
{
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* the VAO is already bound
     * draws two triangles from the vertex buffer,
     * using the element buffer
     * (DRAW_COMMAND, NUMBER, DATA_TYPE, OFFSET) */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
}

#include "../util.h"
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* from https://open.gl/textures */

static GLuint vao; // vertex array object

static GLuint vertexBuffer;
static float vertices[] = {
/* position     | color            | texcoords */
    -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // top-left
    0.5f, 0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // top-right
    0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f   // bottom-left
};

static GLuint elementBuffer;
static GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
};

static GLuint textures[2]; // textures

/* black and white checkerboard */
static float pixels[] = {
    0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f
};

static GLuint vertexShader, fragmentShader;
static GLuint shaderProgram;

static struct timespec tm_spec;

void prog4_init(void)
{
    printf("OpenGL version is %s\n", glGetString(GL_VERSION));
    printf("Demonstration of textures with sampling.\n");

   /* VAOs store links between attributes in a program and raw data.
     * This should come before any calls to glVertexAttribPointer */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* generate VBO */
    glGenBuffers(1, &vertexBuffer);
    printf("vbo = %u\n", vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /* generate EBO */
    glGenBuffers(1, &elementBuffer);
    printf("ebo = %u\n", elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);


    GLint status;
    char err_buf[512];

    int num_lines;
    char **vsh_lines;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    vsh_lines = get_lines("progs/vshader3.glsl", &num_lines);
    glShaderSource(vertexShader, num_lines, (const GLchar **) vsh_lines, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling vshader3.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader vshader3.glsl\n");

    char **fsh_lines;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fsh_lines = get_lines("progs/fragshader4.glsl", &num_lines);
    glShaderSource(fragmentShader, num_lines, (const GLchar **) fsh_lines, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling fragshader4.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader fragshader4.glsl\n");

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
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float),
            (void*)(2*sizeof(float)));

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float),
            (void*)(5*sizeof(float)));

    /* generate textures */
    glGenTextures(2, textures);
    printf("Created texture %u and %u\n", textures[0], textures[1]);

    unsigned char *image;
    int width, height;
    GLenum img_format;

    /* textures[0] will be bound to texture unit 0 */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    /* load an image:
     * param[1] = level of detail (for custom mipmap images)
     * param[2] = *internal* pixel format (RGB = 3 colors)
     * param[3,4] = width and height of image in pixels
     * param[5] = must be 0
     * param[6] = format of the pixels in the given array
     * param[7] = data type of the given array
     * param[8] = the array */
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
    image = load_image("sample.png", &width, &height, &img_format);
    if (image != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, img_format,
                GL_UNSIGNED_BYTE, image);
        printf("Loaded image 1.\n");
    }
    free(image);
    /* set texKitten to use texture unit 0 */
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);
 
    /* parameters and generate mipmap */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* do this for second texture */
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    image = load_image("sample2.png", &width, &height, &img_format);
    if (image != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, img_format,
                GL_UNSIGNED_BYTE, image);
        printf("Loaded image 2.\n");
    }
    free(image);
    /* set texPuppy to use texture unit 0 */
    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

    /* parameters and generate mipmap */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* finally, initialize the interpolated value */
    GLint uniTime = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(uniTime, 0.0f);

    clock_gettime(CLOCK_REALTIME, &tm_spec);
}

void prog4_uninit(void)
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteTextures(2, textures);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteVertexArrays(1, &vao);
}

void prog4_render(void)
{
    struct timespec tmp;
    
    clock_gettime(CLOCK_REALTIME, &tmp);

    double diff_sec = difftime(tmp.tv_sec, tm_spec.tv_sec);
    diff_sec += 1e-9 * (tmp.tv_nsec - tm_spec.tv_nsec);

    GLint uniTime = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(uniTime, diff_sec);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* the VAO is already bound
     * draws two triangles from the vertex buffer,
     * using the element buffer
     * (DRAW_COMMAND, NUMBER, DATA_TYPE, OFFSET) */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
}

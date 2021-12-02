#include "../util.h"
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>

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

static GLuint tex; // texture

/* black and white checkerboard */
static float pixels[] = {
    0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f
};

static GLuint vertexShader, fragmentShader;
static GLuint shaderProgram;

void prog3_init(void)
{
    printf("OpenGL version is %s\n", glGetString(GL_VERSION));
    printf("GLSL version is %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("OpenGL renderer is %s\n", glGetString(GL_RENDERER));
    printf("Demonstration of textures.\n");

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
    fsh_lines = get_lines("progs/fragshader3.glsl", &num_lines);
    glShaderSource(fragmentShader, num_lines, (const GLchar **) fsh_lines, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling fragshader3.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader fragshader3.glsl\n");

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

    /* generate a texture */
    glGenTextures(1, &tex);
    printf("Created texture %u\n", tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* load an image:
     * param[1] = level of detail (for custom mipmap images)
     * param[2] = *internal* pixel format (RGB = 3 colors)
     * param[3,4] = width and height of image in pixels
     * param[5] = must be 0
     * param[6] = format of the pixels in the given array
     * param[7] = data type of the given array
     * param[8] = the array */
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
    unsigned char *image;
    int width, height;
    GLenum img_format;
    image = load_image("cat.jpg", &width, &height, &img_format);
    if (image != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, img_format,
                GL_UNSIGNED_BYTE, image);
        printf("Loaded image.\n");
    }

    /* implicitly, the "tex" uniform is assigned to texture unit 0, which
     * is the default texture unit when we call glBindTexture. To explicitly
     * bind a uniform in the fragment shader to a texture unit, call:
     * glUniform1i(glGetUniformLocation(shaderProgram, "<name>"), UNIT_NUMBER)
     */
 
    /* for textures, (x,y,z) -> (s,t,r). (0,0) is bottom-left and
     * (1,1) is top-right. For 2D textures, we only use (s,t).
     * Here we specify, for our 2D texture, a value for a parameter.
     * ex. GL_TEXTURE_WRAP_S: GL_REPEAT means the wrap behavior of s is
     * GL_REPEAT.
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* if we want to set the border color parameter, we have to
     * specify a float array (fv)
     */
    /*
    float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
    */

    /* filtering */
    /* MIN_FILTER = when scaling the image down.
     * MAG_FILTER = when scaling the image up.
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    /* NOTE: because OpenGL expects the first pixel to be in the bottom-left
     * corner, all images will be flipped when loaded directly. To counteract
     * this, we invert the y-coordinate for textures when drawing.
     */

    /* Generates mipmaps for the texture. The texture
     * must be loaded beforehand. */
    glGenerateMipmap(GL_TEXTURE_2D);
}

void prog3_uninit(void)
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteTextures(1, &tex);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteVertexArrays(1, &vao);
}

void prog3_render(void)
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* the VAO is already bound
     * draws two triangles from the vertex buffer,
     * using the element buffer
     * (DRAW_COMMAND, NUMBER, DATA_TYPE, OFFSET) */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
}

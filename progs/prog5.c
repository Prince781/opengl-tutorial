#include "../util.h"
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gtk/gtk.h>

#include "../math_3d.h"

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

/*  model, view, and projection matrices */
static mat4_t model, view, proj;

extern GtkWidget *gl_area;

void prog5_init(void)
{
    printf("OpenGL version is %s\n", glGetString(GL_VERSION));
    printf("Demonstration of transformations.\n");

    int win_width = gtk_widget_get_allocated_width(gl_area);
    int win_height = gtk_widget_get_allocated_height(gl_area);

    model = m4_rotation(M_PI, vec3(0.0f,0.0f,1.0f));
    /* view matrix:
     * arg1 = position of camera
     * arg2 = center of view
     * arg3 = direction up. Note that because UP is along the z-axis, this causes
     * (x,y)-plane to be perpendicular to "up"
     */
    view = m4_look_at(vec3(1.2f,1.2f,1.2f), vec3(0.0f,0.0f,0.0f), vec3(0.0f,0.0f,1.0f));
    /* projection matrix:
     * arg1 = field of view in degrees
     * arg2 = aspect ratio
     * arg3 = near distance
     * arg4 = far distance
     */
    proj = m4_perspective(60.0f, (float)win_width / win_height, 1.0f, 10.0f);

    printf("model:\n");
    m4_print(model);
    printf("view:\n");
    m4_print(view);
    printf("projection:\n");
    m4_print(proj);

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
    vsh_lines = get_lines("progs/vshader5.glsl", &num_lines);
    glShaderSource(vertexShader, num_lines, (const GLchar **) vsh_lines, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(err_buf), NULL, err_buf);
        fprintf(stderr, "Error compiling vshader5.glsl:\n%s\n", err_buf);
    } else
        printf("Compiled shader vshader5.glsl\n");

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

    /* initialize the model, view, and projection matrices */
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, &model.m[0][0]);
    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, &view.m[0][0]);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, &proj.m[0][0]);

    clock_gettime(CLOCK_REALTIME, &tm_spec);
}

void prog5_uninit(void)
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteTextures(2, textures);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &elementBuffer);
    glDeleteVertexArrays(1, &vao);
}

void prog5_render(void)
{
    int win_width = gtk_widget_get_allocated_width(gl_area);
    int win_height = gtk_widget_get_allocated_height(gl_area);

    struct timespec tmp;
    
    clock_gettime(CLOCK_REALTIME, &tmp);

    double diff_sec = difftime(tmp.tv_sec, tm_spec.tv_sec);
    diff_sec += 1e-9 * (tmp.tv_nsec - tm_spec.tv_nsec);

    GLint uniTime = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(uniTime, diff_sec);

    /* from the vertex shader */
    proj = m4_perspective(60.0f, (float)win_width / win_height, 1.0f, 10.0f);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, &proj.m[0][0]);

    float scale = (1.0f + sinf(diff_sec/4.0 * M_PI))/2.0f;
    scale = 0.3f + 1.4f*powf(scale, 0.4);
    model = m4_rotation(diff_sec/10.0 * M_PI, vec3(0.0f,0.0f,1.0f));
    model = m4_mul(model, m4_scaling(vec3(scale,scale,1.0f)));
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, &model.m[0][0]);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* the VAO is already bound
     * draws two triangles from the vertex buffer,
     * using the element buffer
     * (DRAW_COMMAND, NUMBER, DATA_TYPE, OFFSET) */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glFlush();
}

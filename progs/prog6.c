#include "../util.h"
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <gtk/gtk.h>

#include "../math_3d.h"

/* from https://open.gl/depthstencils */
static GLuint vao; // vertex array object

static GLuint vertexBuffer;
static GLfloat vertices[] = {
/* position(x,y,z)     | color (r,g,b)   | texcoords */
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
// for the floor
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
     1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

static GLuint textures[2]; // textures

static GLuint vertexShader, fragmentShader;
static GLuint shaderProgram;

static struct timespec tm_spec;

/*  model, view, and projection matrices */
static mat4_t model, view, proj;

extern GtkWidget *gl_area;

void prog6_init(void)
{
    printf("OpenGL version is %s\n", glGetString(GL_VERSION));
    printf("GLSL version is %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("OpenGL renderer is %s\n", glGetString(GL_RENDERER));
    printf("Demonstration of depth buffer and stencil buffer.\n");

    int win_width = gtk_widget_get_allocated_width(gl_area);
    int win_height = gtk_widget_get_allocated_height(gl_area);

    gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(gl_area), TRUE);
    printf("Depth buffer enabled? %s\n",
            gtk_gl_area_get_has_depth_buffer(GTK_GL_AREA(gl_area)) ? "yes" : "no");
    gtk_gl_area_set_has_stencil_buffer(GTK_GL_AREA(gl_area), TRUE);
    printf("Stencil enabled? %s\n",
            gtk_gl_area_get_has_stencil_buffer(GTK_GL_AREA(gl_area)) ? "yes" : "no");

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

    vertexShader = shader_new("progs/vshader6.glsl", GL_VERTEX_SHADER);
    fragmentShader = shader_new("progs/fragshader6.glsl", GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    GLint status;
    char err_buf[512];

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
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float),
            (void*)(3*sizeof(float)));

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float),
            (void*)(6*sizeof(float)));

    /* generate textures */
    glGenTextures(2, textures);
    printf("Created texture %u and %u\n", textures[0], textures[1]);

    /* textures[0] will be bound to texture unit 0 */
    glActiveTexture(GL_TEXTURE0);
    texture_load(textures[0], "sample.png");
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
    texture_load(textures[1], "sample2.png");
    /* set texPuppy to use texture unit 1 */
    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

    /* parameters and generate mipmap */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* initialize the model, view, and projection matrices */
    shader_program_set_mat4(shaderProgram, "model", model);
    shader_program_set_mat4(shaderProgram, "view", view);
    shader_program_set_mat4(shaderProgram, "proj", proj);

    clock_gettime(CLOCK_REALTIME, &tm_spec);
}

void prog6_uninit(void)
{
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteTextures(2, textures);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
}

void prog6_render(void)
{
    int win_width = gtk_widget_get_allocated_width(gl_area);
    int win_height = gtk_widget_get_allocated_height(gl_area);

    proj = m4_perspective(60.0f, (float)win_width / win_height, 1.0f, 10.0f);
    shader_program_set_mat4(shaderProgram, "proj", proj);

    struct timespec tmp;
    
    clock_gettime(CLOCK_REALTIME, &tmp);

    double diff_sec = difftime(tmp.tv_sec, tm_spec.tv_sec);
    diff_sec += 1e-9 * (tmp.tv_nsec - tm_spec.tv_nsec);

    /* from the vertex shader */
    model = m4_rotation(diff_sec/4.0 * M_PI, vec3(0.0f,0.0f,1.0f));
    model = m4_mul(model, m4_scaling(vec3(0.5f, 0.5f, 0.5f)));
    shader_program_set_mat4(shaderProgram, "model", model);

    /* By default, all pixels have depth=0, so they are not rendered.
     * We have to clear the depth buffer after enabling depth testing.
     * Everything is initialized with depth=1 then. */
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* the VAO is already bound
     * draws 12 triangles from the vertex buffer
     * (6 faces * 2 tri/face * 3 vert/tri)
     */
    glDrawArrays(GL_TRIANGLES, 0, 36);

    /* drawing the floor and cube reflection */
    glEnable(GL_STENCIL_TEST);
    {
        /* draw floor */
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // always pass
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // set any stencil to 1
        glStencilMask(0xFF); // write to stencil buffer
        glDepthMask(GL_FALSE); // don't write to depth buffer
        glClear(GL_STENCIL_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 36, 6);
        glDepthMask(GL_TRUE);

        /* draw cube reflection */
        /* Pass the test if the stencil value is 1. This means
         * that only areas with stencil=1 will be drawn onto.
         * Since these areas are exactly the areas where the floor
         * was drawn, we will only draw onto the floor.
         */
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDepthMask(GL_TRUE); // write to depth buffer
        mat4_t trans = m4_translation(vec3(0.0f,0.0f,-1.0f));
        mat4_t scale = m4_scaling(vec3(1.0f,1.0f,-1.0f));
        model = m4_mul(model, trans);
        model = m4_mul(model, scale);

        shader_program_set_mat4(shaderProgram, "model", model);
        GLint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
        glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
    }
    glDisable(GL_STENCIL_TEST);

    glDisable(GL_DEPTH_TEST);

    glFlush();
}

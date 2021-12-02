#include <gtk/gtk.h>
#include <math.h>
#include <epoxy/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl_program.h"

GtkWidget *demo_window = NULL;
GtkWidget *gl_area = NULL;
int progID = 0;
guint tick_id = 0;

static void realize(GtkWidget *widget)
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL) {
        fprintf(stderr, "Error!\n");
        return;
    }

    /* TODO: require use of OpenGL (and not OpenGL ES) */
    programs[progID].init();
}

static void unrealize(GtkWidget *widget)
{
    gtk_widget_remove_tick_callback(widget, tick_id);
    gtk_gl_area_make_current(GTK_GL_AREA(widget));

    if (gtk_gl_area_get_error(GTK_GL_AREA(widget)) != NULL) {
        fprintf(stderr, "Error!\n");
        return;
    }

    programs[progID].uninit();
}

static gboolean render(GtkGLArea *area, GdkGLContext *context)
{
    if (gtk_gl_area_get_error(area) != NULL) {
        fprintf(stderr, "Error!\n");
        return FALSE;
    }

    programs[progID].render();

    return TRUE;
}

/* just queues a redraw. render() handles the main graphics issues */
static gboolean update_graphics(GtkWidget *widget, 
        GdkFrameClock *frame_clock, gpointer user_data)
{
    gtk_widget_queue_draw(widget);
    return G_SOURCE_CONTINUE;
}

static void window_close(GtkWidget *widget)
{
    printf("should quit\n");
}

GtkWidget *create_glarea_window(void)
{
    GtkWidget *box;
    GtkWidget *window;

    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "OpenGL Tutorial");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(window_close), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
    gtk_box_set_spacing(GTK_BOX(box), 6);
    gtk_window_set_child(GTK_WINDOW(window), box);

    gl_area = gtk_gl_area_new();
    gtk_widget_set_hexpand(gl_area, TRUE);
    gtk_widget_set_vexpand(gl_area, TRUE);
    gtk_box_append(GTK_BOX(box), gl_area);

    /* gtk_gl_area_set_required_version(GTK_GL_AREA(gl_area), 3, 2); */

    g_signal_connect(gl_area, "realize", G_CALLBACK(realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(unrealize), NULL);
    g_signal_connect(gl_area, "render", G_CALLBACK(render), NULL);
    tick_id = gtk_widget_add_tick_callback(gl_area, update_graphics, NULL, NULL);

    return window;
}

static int command_line (GApplication *self, GApplicationCommandLine *cmdline, void *data)
{
    int argc;
    char **argv;

    argv = g_application_command_line_get_arguments(cmdline, &argc);
    if ((progID = atoi(argv[1])) >= num_programs) {
        fprintf(stderr, "progID must be between 0 and %d\n", num_programs-1);
        fprintf(stderr, "Usage: %s <prog ID>"
                "\n\twhere progID is between 0 and %d\n", argv[0], num_programs-1);
        return 1;
    }

    g_application_activate(self);
    return 0;
}

static void activate(GtkApplication *app, void *data)
{
    if (!demo_window) {
        demo_window = create_glarea_window();
        gtk_application_add_window(app, GTK_WINDOW(demo_window));
    }
    gtk_window_present(GTK_WINDOW(demo_window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    gtk_init();

    app = gtk_application_new("com.github.prince781.OpenGLTutorial", G_APPLICATION_HANDLES_COMMAND_LINE);
    g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}


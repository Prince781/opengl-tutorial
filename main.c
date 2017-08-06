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
    gtk_main_quit();
}

GtkWidget *create_glarea_window(void)
{
    GtkWidget *box;
    GtkWidget *window;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "OpenGL Tutorial");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(window_close), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, FALSE);
    gtk_box_set_spacing(GTK_BOX(box), 6);
    gtk_container_add(GTK_CONTAINER(window), box);

    gl_area = gtk_gl_area_new();
    gtk_widget_set_hexpand(gl_area, TRUE);
    gtk_widget_set_vexpand(gl_area, TRUE);
    gtk_container_add(GTK_CONTAINER(box), gl_area);

    /* gtk_gl_area_set_required_version(GTK_GL_AREA(gl_area), 3, 2); */

    g_signal_connect(gl_area, "realize", G_CALLBACK(realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(unrealize), NULL);
    g_signal_connect(gl_area, "render", G_CALLBACK(render), NULL);
    tick_id = gtk_widget_add_tick_callback(gl_area, update_graphics, NULL, NULL);

    return window;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <prog ID>"
                "\n\twhere progID is between 0 and %d\n", argv[0], num_programs-1);
        return 0;
    }

    if ((progID = atoi(argv[1])) >= num_programs) {
        fprintf(stderr, "progID must be between 0 and %d\n", num_programs-1);
        return 0;
    }

    gtk_init(&argc, &argv);

    demo_window = create_glarea_window();

    gtk_widget_show_all(demo_window);

    gtk_main();
    return 0;
}


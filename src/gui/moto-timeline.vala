using GLib;

public class Moto.Timeline : Gtk.DrawingArea
{
    private float fps;
    private float current_time;

    construct
    {
        /* data */
        fps = 24;
        current_time = 0;

        /* signals */
        expose_event    += expose_event_cb;
        button_press    += button_press_cb;
        button_release  += button_release_cb;
        motion_notify   += motion_notify_cb;

    }

    private bool expose_event_cb(Gtk.Widget widget, Gdk.Event event)
    {
        Cairo.Context ctx = Gdk.cairo_create(this.window);

        ctx.set_cource_rgb(0, 0, 0);

        return true;
    }
}

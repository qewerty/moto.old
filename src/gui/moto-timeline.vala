using GLib;

public class Moto.Timeline : Gtk.DrawingArea
{
    private float fps {get; set;}

    private float start_time {get; set;}
    private float end_time {get; set;}
    private float current_time {get; set;} // in seconds

    private float start_draw_time {get; set;}
    private float end_draw_time {get; set;}

    /* signals */

    public signal void frame_changed(float prev_time);

    construct
    {
        /* data */

        fps = 24;

        start_time      = 0;
        end_time        = 100;
        current_time    = 0;

        start_draw_time = 0;
        end_draw_time   = 0;

        /* signal callbacks */

        expose_event    += expose_event_cb;
        // button_press    += button_press_cb;
        // button_release  += button_release_cb;
        // motion_notify   += motion_notify_cb;
    }

    public void set_frame(int frame)
    {
        float prev_time = current_time;
        current_time = frame/fps;

        frame_changed(prev_time);
    }

    public void next_frame()
    {
        float prev_time = current_time;
        current_time += 1/fps;

        frame_changed(prev_time);
    }
    public void prev_frame()
    {
        float prev_time = current_time;
        current_time -= 1/fps;

        frame_changed(prev_time);
    }

    /* singal callbacks */

    private bool expose_event_cb(Gtk.Widget widget, Gdk.Event event)
    {
        Cairo.Context ctx = Gdk.cairo_create(this.window);

        // ctx.set_cource_rgb(0, 0, 0);

        return true;
    }
}

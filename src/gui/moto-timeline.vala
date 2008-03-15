using GLib;

public class Moto.Timeline : Gtk.DrawingArea
{
    private float _fps;

    private float _start_time;
    private float _end_time;
    private float _current_time; // in seconds

    private float _start_draw_time;
    private float _end_draw_time;

    /* signals */

    public signal void frame_changed(float prev_time);

    construct
    {
        /* data */

        _fps = 24;

        _start_time      = 0;
        _end_time        = 100;
        _current_time    = 0;

        _start_draw_time = 0;
        _end_draw_time   = 0;

        /* signal callbacks */

        expose_event    += expose_event_cb;
        button_press    += button_press_cb;
        button_release  += button_release_cb;
        motion_notify   += motion_notify_cb;
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

        ctx.set_cource_rgb(0, 0, 0);

        return true;
    }
}

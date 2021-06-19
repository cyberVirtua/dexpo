#include "config.hpp"
#include "desktop_pixmap.hpp"
#include "dexpo_socket.hpp"
#include "window.hpp"
#include "wmctrl.hpp"
#include <unistd.h>
#include <vector>
desktop_pixmap d1 (0, 0, 1920, 1080, "DP-1");
desktop_pixmap d2 (0, 0, 1920, 1080, "DP-2");
desktop_pixmap d3 (0, 0, 1920, 1080, "DP-3");

// Used for testing purposes
std::vector<dexpo_pixmap>
test_get_vector ()
{
  d1.save_screen ();
  d2.save_screen ();
  d3.save_screen ();

  dexpo_pixmap d1_1{ 0, d1.pixmap_width, d1.pixmap_height, d1.pixmap_id,
                     "DP-1" };
  dexpo_pixmap d2_1{ 1, d2.pixmap_width, d2.pixmap_height, d2.pixmap_id,
                     "DP-2" };
  dexpo_pixmap d3_1{ 2, d3.pixmap_width, d3.pixmap_height, d3.pixmap_id,
                     "DP-3" };
  return std::vector<dexpo_pixmap>{ d1_1, d2_1, d3_1 };
}

int
main ()
{
  // Gets width, height and screenshot of every desktop
  auto v = test_get_vector ();

  // Calculates size of GUI's window
  auto conf_width = dexpo_width;   // Width from config
  auto conf_height = dexpo_height; // Height from config
  if (conf_width == 0)
    {
      conf_width += dexpo_padding; // Add the padding before first screenshot
      conf_height
          += 2 * dexpo_padding; // Add the paddng at both sizes of interface
      for (const auto &dexpo_pixmap : v)
        {
          conf_width += dexpo_pixmap.width;
          conf_width += dexpo_padding;
        }
    }
  else if (conf_height == 0)
    {
      conf_height += dexpo_padding; // Add the padding before first screenshot
      conf_width
          += 2 * dexpo_padding; // Add the paddng at both sizes of interface
      for (const auto &dexpo_pixmap : v)
        {
          conf_height += dexpo_pixmap.height;
          conf_height += dexpo_padding;
        }
    };

  window w (dexpo_x, dexpo_y, conf_width, conf_height);
  w.pixmaps = v;
  // Mapping pixmap onto window
  xcb_generic_event_t *event;
  while ((event = xcb_wait_for_event (window::c_)))
    {
      switch (event->response_type & ~0x80)
        {
        case XCB_EXPOSE:
          {
            w.draw_gui ();
            w.highlight_window (0, dexpo_hlcolor);
            break;
          }
        case XCB_KEY_PRESS:
          {
            xcb_key_press_event_t *kp
                = reinterpret_cast<xcb_key_press_event_t *> (event);
            ;
            if (kp->detail == 114
                or kp->detail == 116) // right arrow or up arrow
              {
                w.highlight_window (w.highlighted, dexpo_bgcolor);
                w.highlighted += 1;
                w.highlighted = w.highlighted % int (v.size ());
                w.highlight_window (w.highlighted, dexpo_hlcolor);
              }
            if (kp->detail == 113
                or kp->detail == 111) // left arrow or down arrow
              {
                w.highlight_window (w.highlighted, dexpo_bgcolor);
                w.highlighted = (w.highlighted == 0) ? int (v.size () - 1)
                                                     : w.highlighted - 1;
              }
            w.highlight_window (w.highlighted, dexpo_hlcolor);
            if (kp->detail == 9) // escape
              {

                exit (0);
              }
            if (kp->detail == 36) // enter

              {
                w.highlight_window (w.highlighted, dexpo_bgcolor);
                ewmh_change_desktop (window::c_, window::screen_,
                                     w.highlighted);
              }
          }
          break;
        case XCB_MOTION_NOTIFY: // pointer motion within window
          {
            xcb_motion_notify_event_t *mn
                = reinterpret_cast<xcb_motion_notify_event_t *> (event);
            int det = -1;
            // TODO: Keep coordinates of pixmap in window class instead
            // of this hardcoding
            for (const auto &dexpo_pixmap : w.pixmaps)
              {
                if (dexpo_height == 0)
                  {
                    if ((mn->event_x - dexpo_padding > 0)
                        and (mn->event_x - dexpo_padding < dexpo_pixmap.width)
                        and (mn->event_y
                                 - w.get_screen_position (
                                     dexpo_pixmap.desktop_number)
                             > 0)
                        and (mn->event_y
                                 - w.get_screen_position (
                                     dexpo_pixmap.desktop_number)
                             < dexpo_pixmap.height))
                      {
                        det = dexpo_pixmap.desktop_number;
                        break;
                      }
                  }
                else if (dexpo_width == 0)
                  {
                    if ((mn->event_x
                             - w.get_screen_position (
                                 dexpo_pixmap.desktop_number)
                         > 0)
                        and (mn->event_x
                                 - w.get_screen_position (
                                     dexpo_pixmap.desktop_number)
                             < dexpo_pixmap.width)
                        and (mn->event_y - dexpo_padding > 0)
                        and (mn->event_y - dexpo_padding < dexpo_pixmap.height))
                      {
                        det = dexpo_pixmap.desktop_number;
                        break;
                      }
                  }
              }
            w.highlight_window (w.highlighted, dexpo_bgcolor);
            if (det > -1)
              {
                w.highlighted = det;
                w.highlight_window (w.highlighted, dexpo_hlcolor);
              }
            break;
          }
        case XCB_ENTER_NOTIFY: // pointer enters window
          {
            xcb_set_input_focus (window::c_, XCB_INPUT_FOCUS_POINTER_ROOT, w.id,
                                 XCB_TIME_CURRENT_TIME);
            break;
          }
        case XCB_LEAVE_NOTIFY: // pointer leaves window
          {
            w.highlight_window (w.highlighted, dexpo_bgcolor);
            xcb_set_input_focus (window::c_, XCB_INPUT_FOCUS_POINTER_ROOT,
                                 window::screen_->root, XCB_TIME_CURRENT_TIME);
            break;
          }
        case XCB_BUTTON_PRESS: // mouse click
          {
            w.highlight_window (w.highlighted, dexpo_bgcolor);
            ewmh_change_desktop (window::c_, window::screen_, w.highlighted);
          }
        }
      xcb_flush (window::c_);
    }
}
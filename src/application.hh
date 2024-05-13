#include <gtkmm.h>

#include "list.hh"
#include "opencv2/img_hash/color_moment_hash.hpp"

class CW1::Application : public Gtk::Application {
 private:
  CW1::List<cv::img_hash::ColorMomentHash> list;
  void on_open_clicked();
  void on_open_stop(Glib::RefPtr<Gio::AsyncResult>& result);
  Glib::RefPtr<Gtk::FileDialog> on_open_dialog;

 protected:
  Application();
  void on_activate() override;
  int on_command_line(
      const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) override;

 public:
  static Glib::RefPtr<Application> create();
};

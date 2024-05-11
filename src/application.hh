#include <gtkmm.h>

#include "main.hh"

class CW1::Application : public Gtk::Application {
 private:
  CW1::List list;
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

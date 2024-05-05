#include <gtkmm.h>

#include "main.hh"

class CW1::Application : public Gtk::Application {
 protected:
  Application();
  void on_activate() override;
  int on_command_line(
      const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) override;

 public:
  static Glib::RefPtr<Application> create();
};

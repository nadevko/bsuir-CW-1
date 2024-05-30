#pragma once

#include <gtkmm/application.h>

#include "main.hh"

namespace CW1 {

class Application : public Gtk::Application {
 public:
  Application();
  static ref<Application> create();

 protected:
  int on_command_line(const ref<Gio::ApplicationCommandLine> &cli) override;
};

}  // namespace CW1

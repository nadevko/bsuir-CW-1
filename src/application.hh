#pragma once

#include <gtkmm/application.h>

#include "main.hh"
#include "rvhash.hh"

namespace CW1 {

class Application : public Gtk::Application {
 public:
  Application();
  static ref<Application> create();

 protected:
  int on_command_line(const ref<Gio::ApplicationCommandLine> &cli) override;
  RVHash rvhash;

 private:
  Glib::OptionGroup optionsFilters;
  int side = 8;
  int bins = 30;
  int sectors = 180;
  double sigma = 1;
  double stdDeviationThreshold = 10;
  double medianThreshold = 10;
};

}  // namespace CW1

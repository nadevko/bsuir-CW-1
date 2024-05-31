#pragma once

#include <gtkmm/application.h>

#include "glibmm/refptr.h"
#include "rvhash.hh"

namespace CW1 {

class Application : public Gtk::Application {
    
 public:
  Application();
  static Glib::RefPtr<Application> create();

 protected:
  int on_command_line(
      const Glib::RefPtr<Gio::ApplicationCommandLine>& cli) override;
  std::vector<std::string> process_options(
      const Glib::RefPtr<Glib::VariantDict>& options);

 private:

  bool recursive = false;
  Glib::OptionGroup optionsFilters;
  int side = 8;
  int bins = 30;
  int sectors = 180;
  double sigma = 1;
  double stdDeviationThreshold = 10;
  double medianThreshold = 10;
  RVHash rvhash;

};

}  // namespace CW1

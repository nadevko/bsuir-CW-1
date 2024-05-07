#include "application.hh"

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <iostream>
#include <opencv2/opencv.hpp>

#include "image.hh"

// make new app instance
CW1::Application::Application()
    : Gtk::Application("org.gtkmm.examples.application",
                       Gio::Application::Flags::HANDLES_OPEN |
                           Gio::Application::Flags::HANDLES_COMMAND_LINE) {
  // set pretty name
  Glib::set_application_name("CW1");

  // define options
  add_main_option_entry(Gtk::Application::OptionType::BOOL, "version", 'v',
                        _("Display version information and exit"));
  add_main_option_entry(Gtk::Application::OptionType::FILENAME_VECTOR,
                        G_OPTION_REMAINING);
}

Glib::RefPtr<CW1::Application> CW1::Application::create() {
  return Glib::make_refptr_for_instance<Application>(new Application());
}

// cli session
int CW1::Application::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line) {
  const auto options = command_line->get_options_dict();

  bool version = false;
  options->lookup_value("version", version);

  if (version) {
    std::cout << _(
        "CW1 (courcework 1) 1.0\nCopyright (C) 2024 nadevko\nLicense GPLv3: "
        "GNU "
        "GPL version 3 <https://gnu.org/licenses/gpl.html>.\nThis is free "
        "software: you are free to change and redistribute it.\nThere is NO "
        "WARRANTY, to the extent permitted by law.\n");
    return EXIT_SUCCESS;
  }

  std::vector<std::string> remaining;
  options->lookup_value(G_OPTION_REMAINING, remaining);

  switch (remaining.size()) {
    case 0: {
      activate();
      return EXIT_SUCCESS;
    }
    case 1: {
      return EXIT_FAILURE;
    }
    default: {
      std::vector<CW1::Image> images;
      for (auto path : remaining)
        images.push_back(CW1::Image(Gio::File::create_for_path(path)));
      for (auto i = images.begin(); i != images.end(); i++)
        for (auto j = images.begin(); j != i; j++)
          std::cout << i->file->get_path() << " " << j->file->get_path() << " "
                    << i->compare(*j) << "\n";
      return EXIT_SUCCESS;
    }
  }
}

// gui session
void CW1::Application::on_activate() {
  auto builder = Gtk::Builder::create();
  builder->add_from_resource("/io/github/nadevko/cw1/views/application.ui");
  auto window = builder->get_widget<Gtk::Window>("application");
  this->add_window(*window);
  window->set_visible();
}

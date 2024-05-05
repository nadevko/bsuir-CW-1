#include "application.hh"

#include <glib/gi18n.h>

#include <iostream>

// make new app instance
CW1::Application::Application()
    : Gtk::Application("org.gtkmm.examples.application",
                       Gio::Application::Flags::HANDLES_OPEN |
                           Gio::Application::Flags::HANDLES_COMMAND_LINE) {
  // set pretty name
  Glib::set_application_name("CW1");

  // define arguments
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

  Glib::OptionGroup::vecstrings remaining;
  options->lookup_value(G_OPTION_REMAINING, remaining);

  if (remaining.empty()) activate();
  return EXIT_SUCCESS;
}

// gui session
void CW1::Application::on_activate() {
  auto builder = Gtk::Builder::create();
  builder->add_from_resource("/io/github/nadevko/cw1/views/about.ui");
  auto about = builder->get_widget<Gtk::AboutDialog>("About");
  this->add_window(*about);
  about->set_visible();
}

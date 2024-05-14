#include "application.hh"

#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include <iostream>
#include <opencv2/opencv.hpp>

#include "image.hh"
#include "main.hh"

using Hasher = cv::img_hash::ColorMomentHash;

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
  add_main_option_entry(Gtk::Application::OptionType::DOUBLE, "percentage", 'p',
                        _("Minimal simularity to remove (0-100)"));
  add_main_option_entry(Gtk::Application::OptionType::FILENAME, "output", 'o',
                        _("Override output script name (delete.sh)"));
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
    }
    case 1: {
      auto root = Gio::File::create_for_path(remaining[0]);
      list = CW1::List<Hasher>(root);
      for (auto i : list)
        std::cout << root->get_relative_path(i.first.first->file)
                  << " :: "
                  << root->get_relative_path(i.first.second->file)
                  << " : " << list.to_percent(i.second) << "\n";
    }
    default: {
      std::vector<CW1::Image<Hasher>> images;
      for (auto path : remaining)
        images.push_back(CW1::Image<Hasher>(Gio::File::create_for_path(path)));
      for (auto i = images.begin(); i != images.end(); i++)
        for (auto j = images.begin(); j != i; j++)
          std::cout << i->file->get_path() << " " << j->file->get_path()
                    << " :: " << i->compare(*j) << "\n";
    }
  }
  return EXIT_SUCCESS;
}

void CW1::Application::on_open_clicked() {
  if (!on_open_dialog) {
    std::cout << "cc\n";
    on_open_dialog = Gtk::FileDialog::create();
    on_open_dialog->set_modal(true);
  }
  on_open_dialog->select_folder(
      sigc::mem_fun(*this, &CW1::Application::on_open_stop));
}

void CW1::Application::on_open_stop(Glib::RefPtr<Gio::AsyncResult>& result) {
  Glib::RefPtr<Gio::File> directory;
  try {
    directory = on_open_dialog->select_folder_finish(result);
  } catch (const Gtk::DialogError& err) {
    std::cerr << _("Directory not selected\n");
  }
  list.set_root(directory);
}

// gui session
void CW1::Application::on_activate() {
  // load builder
  auto builder = Gtk::Builder::create();
  builder->add_from_resource("/io/github/nadevko/cw1/views/application.ui");
  auto window = builder->get_widget<Gtk::Window>("application");
  // attach signals
  auto open = builder->get_widget<Gtk::Button>("application_open");
  open->signal_clicked().connect(
      sigc::mem_fun(*this, &CW1::Application::on_open_clicked));
  // make window
  this->add_window(*window);
  window->set_visible();
}

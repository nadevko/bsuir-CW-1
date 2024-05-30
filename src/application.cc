#include "application.hh"

#include <glibmm/miscutils.h>

#include <iomanip>
#include <iostream>

#include "glibmm/fileutils.h"
#include "glibmm/optioncontext.h"
#include "rvhash.hh"

namespace CW1 {

Application::Application()
    : Gtk::Application("io.github.nadevko.cw1",
                       Gio::Application::Flags::HANDLES_COMMAND_LINE) {
  Glib::set_application_name("cw1");

  // define options
  add_main_option_entry(Gtk::Application::OptionType::BOOL, "version", 'v',
                        "Display version information and exit");
  add_main_option_entry(Gtk::Application::OptionType::FILENAME_VECTOR,
                        G_OPTION_REMAINING);
}

ref<Application> Application::create() {
  return Glib::make_refptr_for_instance<Application>(new Application());
}

int Application::on_command_line(const ref<Gio::ApplicationCommandLine>& cli) {
  auto options = cli->get_options_dict();

  std::vector<std::string> filepaths;
  options->lookup_value(G_OPTION_REMAINING, filepaths);

  if (filepaths.empty())
    throw Glib::OptionError(Glib::OptionError::BAD_VALUE,
                            "Pass at least one filepath");

  CW1::RVHash rvhash;
  auto basePath = filepaths[0];
  auto base = rvhash.compute(Gdk::Pixbuf::create_from_file(basePath));
  filepaths.erase(filepaths.cbegin());

  if (filepaths.size() == 0) {
    std::cout << basePath << " | " << base << "\n";
    return 0;
  }

  size_t longestLength =
      (*std::max_element(filepaths.begin(), filepaths.end(),
                         [](const std::string& a, const std::string& b) {
                           return a.size() < b.size();
                         }))
          .size();

  for (const auto& path : filepaths) try {
      auto pixbuf = Gdk::Pixbuf::create_from_file(path);
      auto pattern = rvhash.compute(pixbuf);
      std::cout << std::setw(longestLength) << std::left << path << " | "
                << pattern << " | " << rvhash.compare(base, pattern) << '\n';
    } catch (const Glib::FileError& ex) {
      std::cerr << "Error loading image " << path << ": " << ex.what() << '\n';
    }
  return 0;
}

}  // namespace CW1

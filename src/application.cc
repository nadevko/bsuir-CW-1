#include "application.hh"

#include <glibmm.h>

#include <iomanip>
#include <iostream>

#include "glibmm/fileutils.h"
#include "glibmm/optioncontext.h"
#include "glibmm/optionentry.h"
#include "glibmm/optiongroup.h"
#include "rvhash.hh"

namespace CW1 {

Application::Application()
    : Gtk::Application("io.github.nadevko.cw1",
                       Gio::Application::Flags::HANDLES_COMMAND_LINE),
      optionsFilters("hash", "Hashing algorithm options:",
                     "Show hashing algorith options") {
  Glib::set_application_name("cw1");

  // define options
  add_main_option_entry(Gtk::Application::OptionType::BOOL, "version", 'v',
                        "Display program version and exit");
  add_main_option_entry(Gtk::Application::OptionType::BOOL, "base", 'b',
                        "Compare all images only with first");
  add_main_option_entry(Gtk::Application::OptionType::STRING, "output", 'o',
                        "Filename to write output", "filepath");
  add_main_option_entry(
      Gtk::Application::OptionType::STRING, "export", 'e',
      "Set results export format\n"
      "   txt, plaintext            Pretty-formatted plaintext\n"
      "    sh, shell                Linux POSIX-compilant shell\n"
      "        json                 JavaScript Object Notation\n"
      "   csv, comma                Comma separated values",
      "format");
  add_main_option_entry(Gtk::Application::OptionType::FILENAME_VECTOR,
                        G_OPTION_REMAINING);

  Glib::OptionEntry entry;

  entry.set_long_name("side");
  entry.set_description("Set length of resized image side");
  entry.set_arg_description("uint");
  optionsFilters.add_entry(entry, side);

  entry.set_long_name("bins");
  entry.set_short_name('b');
  entry.set_description("Set bins size");
  entry.set_arg_description("uint");
  optionsFilters.add_entry(entry, bins);

  entry.set_long_name("sectors");
  entry.set_short_name('s');
  entry.set_description("Set number of sectors");
  entry.set_arg_description("uint");
  optionsFilters.add_entry(entry, sectors);

  entry.set_long_name("sigma");
  entry.set_description("Set sigma value");
  entry.set_arg_description("float");
  optionsFilters.add_entry(entry, sigma);

  entry.set_long_name("deviation");
  entry.set_description("Set standard deviation threshold");
  entry.set_arg_description("float");
  optionsFilters.add_entry(entry, stdDeviationThreshold);

  entry.set_long_name("median");
  entry.set_description("Set median threshold");
  entry.set_arg_description("float");
  optionsFilters.add_entry(entry, medianThreshold);

  add_option_group(optionsFilters);
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

  rvhash.set_size(side);
  rvhash.set_bins(bins);
  rvhash.set_sectors(sectors);
  rvhash.set_medianThreshold(medianThreshold);
  rvhash.set_stdDeviationThreshold(stdDeviationThreshold);

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

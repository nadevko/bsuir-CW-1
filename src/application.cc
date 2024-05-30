#include "application.hh"

#include <glibmm.h>

#include <iostream>

#include "glibmm/optioncontext.h"
#include "glibmm/optionentry.h"
#include "glibmm/optiongroup.h"

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
  add_main_option_entry(Gtk::Application::OptionType::BOOL, "recursive", 'r',
                        "Is directories should be processed recursively");
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
  auto filepaths =
      process_options(options);  // Process options and get filepaths

  if (filepaths.empty())
    throw Glib::OptionError(Glib::OptionError::BAD_VALUE,
                            "Pass at least one filepath");

  process_filepaths(filepaths);  // Process filepaths

  return 0;
}

std::vector<std::string> Application::process_options(
    const Glib::RefPtr<Glib::VariantDict>& options) {
  std::vector<std::string> filepaths;
  options->lookup_value("recursive", recursive);
  options->lookup_value(G_OPTION_REMAINING, filepaths);

  rvhash.set_size(side);
  rvhash.set_bins(bins);
  rvhash.set_sectors(sectors);
  rvhash.set_medianThreshold(medianThreshold);
  rvhash.set_stdDeviationThreshold(stdDeviationThreshold);

  return filepaths;
}

void Application::process_filepaths(
    const std::vector<std::string>& filepaths) const {
  for (const auto& path : filepaths) {
    if (std::filesystem::is_directory(path)) {
      std::visit(
          [this](auto&& it) {
            for (const auto& entry : it) {
              if (std::filesystem::is_regular_file(entry.path())) {
                process_file(entry.path().string());
              }
            }
          },
          recursive ? iterator_type(
                          std::filesystem::recursive_directory_iterator(path))
                    : iterator_type(std::filesystem::directory_iterator(path)));
    } else {
      process_file(path);
    }
  }
}

void Application::process_file(const std::string& filepath) const {
  if (!is_image_file(filepath)) return;
  auto pixbuf = Gdk::Pixbuf::create_from_file(filepath);
  auto pattern = rvhash.compute(pixbuf);
  std::cout << filepath << " | " << pattern << '\n';
}

bool Application::is_image_file(const std::string& filepath) const {
  try {
    // Create a Gio::File instance for the file path
    auto file = Gio::File::create_for_path(filepath);

    // Get the file info to retrieve MIME type
    auto file_info = file->query_info("standard::content-type");

    // Get the MIME type of the file
    auto mime_type = file_info->get_content_type();

    // Check if the MIME type indicates an image
    return mime_type.substr(0, 6) == "image/";
  } catch (const Glib::Error& ex) {
    // Failed to get file info or MIME type, treat as non-image
    return false;
  }
}

}  // namespace CW1

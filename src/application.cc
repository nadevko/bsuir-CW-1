#include "application.hh"

#include <glibmm.h>
#include <glibmm/optioncontext.h>
#include <glibmm/optionentry.h>
#include <glibmm/optiongroup.h>
#include <glibmm/refptr.h>

#include <iostream>

#include "hashformatter.hh"
#include "hashprocessor.hh"

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
                        "Process directories recursively");
  add_main_option_entry(
      Gtk::Application::OptionType::STRING, "mode", 'm',
      "Mode of program execution"
      "     h, hash                 Only compute hashes\n"
      "     a, all                  All image comparison pairs\n"
      "     f, first                Compare every image with first in list");
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

Glib::RefPtr<Application> Application::create() {
  return Glib::make_refptr_for_instance<Application>(new Application());
}

int Application::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine>& cli) {
  auto options = cli->get_options_dict();
  auto filepaths =
      process_options(options);  // Process options and get filepaths

  if (filepaths.empty())
    throw Glib::OptionError(Glib::OptionError::BAD_VALUE,
                            "Pass at least one filepath");

  // Use HashProcessor to process filepaths
  HashProcessor processor;
  auto formatter = std::make_shared<const HashFormatter>();
  processor.set_size(side);
  processor.set_bins(bins);
  processor.set_sectors(sectors);
  processor.set_medianThreshold(medianThreshold);
  processor.set_stdDeviationThreshold(stdDeviationThreshold);
  processor.set_recursive(recursive);
  processor.set_formatter(formatter);

  auto table = processor.process_filepaths(
      filepaths);  // Process filepaths using HashProcessor
  processor.format(std::cout, table);

  return 0;
}

std::vector<std::string> Application::process_options(
    const Glib::RefPtr<Glib::VariantDict>& options) {
  std::vector<std::string> filepaths;
  options->lookup_value("recursive", recursive);
  options->lookup_value(G_OPTION_REMAINING, filepaths);

  return filepaths;
}

}  // namespace CW1

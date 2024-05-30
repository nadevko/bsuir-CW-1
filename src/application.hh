#pragma once

#include <gtkmm/application.h>

#include <filesystem>
#include <variant>

#include "main.hh"
#include "rvhash.hh"

namespace CW1 {

class Application : public Gtk::Application {
 public:
  Application();
  static ref<Application> create();

 protected:
  int on_command_line(const ref<Gio::ApplicationCommandLine>& cli) override;
  std::vector<std::string> process_options(
      const Glib::RefPtr<Glib::VariantDict>& options);
  void process_filepaths(const std::vector<std::string>& filepaths) const;
  void process_filepaths(
      const std::filesystem::directory_iterator& dir_iter) const;
  void process_file(const std::string& filepath) const;
  bool is_image_file(const std::string& filepath) const;
  RVHash rvhash;

 private:
  using iterator_type =
      std::variant<std::filesystem::directory_iterator,
                   std::filesystem::recursive_directory_iterator>;

  bool recursive = false;
  Glib::OptionGroup optionsFilters;
  int side = 8;
  int bins = 30;
  int sectors = 180;
  double sigma = 1;
  double stdDeviationThreshold = 10;
  double medianThreshold = 10;
};

}  // namespace CW1

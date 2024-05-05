#include <glib/gi18n.h>
#include <gtkmm.h>

#include <iostream>

#include "config.h"

Glib::RefPtr<Gtk::Application> app;

namespace CW1 {
class OptionGroup;
};

class CW1::OptionGroup : public Glib::OptionGroup {
 private:
  bool on_option_version(const Glib::ustring& option_name,
                         const Glib::ustring& value, bool has_value);

 public:
  OptionGroup();
  Glib::ustring arg_version;
  Glib::OptionGroup::vecstrings arg_remaining;
};

CW1::OptionGroup::OptionGroup()
    : Glib::OptionGroup("example_group", "description of example group",
                        "help description of example group") {
  Glib::OptionEntry version;
  version.set_long_name("version");
  version.set_short_name('v');
  version.set_description(_("Display version information and exit"));
  version.set_flags(Glib::OptionEntry::Flags::OPTIONAL_ARG);
  arg_version = "not specified";
  add_entry(version,
            sigc::mem_fun(*this, &CW1::OptionGroup::on_option_version));

  Glib::OptionEntry remaining;
  remaining.set_long_name(G_OPTION_REMAINING);
  add_entry_filename(remaining, arg_remaining);
}

bool CW1::OptionGroup::on_option_version(const Glib::ustring& option_name,
                                         const Glib::ustring& value,
                                         const bool has_value) {
  if (option_name != "-v" && option_name != "--version")
    throw Glib::OptionError(Glib::OptionError::UNKNOWN_OPTION, arg_version);
  std::cout << _(
      "CW1 (courcework 1) 1.0\nCopyright (C) 2024 nadevko\nLicense GPLv3: GNU "
      "GPL version 3 <https://gnu.org/licenses/gpl.html>.\nThis is free "
      "software: you are free to change and redistribute it.\nThere is NO "
      "WARRANTY, to the extent permitted by law.\n");
  exit(0);
}

int main(int argc, char** argv) {
  // i18n init
  setlocale(LC_ALL, "");
  bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
  textdomain(GETTEXT_PACKAGE);

  // application init
  app = Gtk::Application::create("io.github.nadevko.cw1");
  // parse options
  Glib::OptionContext context;
  CW1::OptionGroup group;
  context.set_main_group(group);

  try {
    context.parse(argc, argv);
  } catch (const Glib::Error& err) {
    std::cerr << err.what() << "\n";
    exit(err.code());
  }

  // run application
  app->signal_activate().connect([]() {
    // run gui session
    auto builder = Gtk::Builder::create();
    builder->add_from_resource("/io/github/nadevko/cw1/views/about.ui");
    auto about = builder->get_widget<Gtk::AboutDialog>("About");
    app->add_window(*about);
    about->set_visible();
  });

  return app->run(argc, argv);
}

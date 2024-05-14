#include <adwaita.h>
#include <gtkmm.h>

#include "application.hh"
#include "config.h"

int main(int argc, char** argv) {
  // i18n init
  setlocale(LC_ALL, "");
  bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
  textdomain(GETTEXT_PACKAGE);
  // adwaita init
  adw_init();
  // application init
  static auto app = CW1::Application::create();
  // run applicaton
  return app->run(argc, argv);
}

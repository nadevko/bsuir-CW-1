#include <adwaita.h>
#include <glibmm/i18n.h>

#include "application.hh"
#include "config.h"

int main(int argc, char** argv) {
  setlocale(LC_ALL, "");
  bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
  textdomain(GETTEXT_PACKAGE);
  adw_init();
  static auto app = CW1::Application::create();
  return app->run(argc, argv);
}

#include <iostream>

#include "gtkmm/application.h"
#include "gtkmm/applicationwindow.h"
#include "gtkmm/builder.h"

Glib::RefPtr<Gtk::Application> app;

void Echo() { std::cout << "baka"; }

int main(int argc, char** argv) {
  if (argc > 1 || !gtk_init_check()) {
    std::cerr << "Program supports only gui mode and zero args\n";
    return 1;
  }

  app = Gtk::Application::create("io.github.nadevko.cw1");

  app->signal_activate().connect([]() {
    auto builder = Gtk::Builder::create();
    builder->add_from_resource("/io/github/nadevko/cw1/views/application.ui");
    auto window = builder->get_widget<Gtk::ApplicationWindow>("Application");
    app->add_window(*window);
    window->set_visible();
  });

  return app->run(argc, argv);
}

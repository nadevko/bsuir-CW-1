#include "gtkmm/aboutdialog.h"
#include "gtkmm/application.h"
#include "gtkmm/builder.h"

Glib::RefPtr<Gtk::Application> app;

int main(int argc, char** argv) {
  app = Gtk::Application::create("io.github.nadevko.cw1");

  app->signal_activate().connect([]() {
    auto builder = Gtk::Builder::create();
    builder->add_from_resource("/io/github/nadevko/cw1/views/about.ui");
    auto about = builder->get_widget<Gtk::AboutDialog>("About");
    app->add_window(*about);
    about->set_visible();
  });

  return app->run(argc, argv);
}

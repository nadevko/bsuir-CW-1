#include <gtkmm.h>

Glib::RefPtr<Gtk::Application> app;

int
main(int argc, char** argv)
{
  app = Gtk::Application::create("io.github.nadevko.cw1");

  app->signal_activate().connect([]() {
    auto builder = Gtk::Builder::create();
    builder->add_from_file("../src/views/about.ui");
    auto about = builder->get_widget<Gtk::AboutDialog>("about");
    app->add_window(*about);
    about->set_visible(true);
  });

  return app->run(argc, argv);
}

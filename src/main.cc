#include <gtkmm.h>
#include <iostream>

Glib::RefPtr<Gtk::Application> app;

int
main(int argc, char** argv)
{
  if (argc > 1 || !gtk_init_check()) {
    std::cerr << "Program supports only gui mode and zero args\n";
    return 1;
  }

  app = Gtk::Application::create("io.github.nadevko.cw1");

  app->signal_activate().connect([]() {
    auto builder = Gtk::Builder::create();
    builder->add_from_resource("/io/github/nadevko/cw1/views/about.ui");
    auto about = builder->get_widget<Gtk::AboutDialog>("about");
    app->add_window(*about);
    about->set_visible(true);
  });

  return app->run(argc, argv);
}

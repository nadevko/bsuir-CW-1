#include <gtkmm.h>

int main(int argc, char** argv) {
  // test framework init
  gtk_test_init(&argc, &argv);
  // body
  // run tests
  return g_test_run();
}

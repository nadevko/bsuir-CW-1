#include <getopt.h>
#include <iostream>

struct options
{
  int* help = 0;
};

int
ReadOptions(int argc, char** argv)
{
  while (true) {
    options flags;
    char short_options[] = "?v";
    struct option long_options[] = { { "help", no_argument, flags.help, 'h' },
                                     { "version", no_argument, 0, 'v' },
                                     { 0, 0, 0, 0 } };
    int option_index = 0;
    int c = getopt_long(argc, argv, short_options, long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case 0:
        if (long_options[option_index].flag != 0)
          break;
        printf("option %s", long_options[option_index].name);
        if (optarg)
          printf(" with arg %s", optarg);
        printf("\n");
        break;

      case 'v':
        std::cout << "version\n";
        break;

      case 'h':
        std::cout << "help\n";
        return 1;

      case '?':
      default:
        std::cout << "help\n";
        return 1;
    }
  }
  return 0;
}

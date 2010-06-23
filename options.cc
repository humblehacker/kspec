#include <iostream>
#include <stdexcept>
#include <getopt.h>

#include "Options.h"

using std::cout;
using std::cerr;
using std::endl;

static struct option long_options[] =
{
    { "pdf", no_argument, 0, 'p' },
    { "version", no_argument, 0, 'v' },
    { "help", no_argument, 0, 'h' },
    { 0, 0, 0, 0 }
};

Options::
Options(int argc, char *argv[])
{
  int option_index = 0;

  int ch;
  while ((ch = getopt_long(argc, argv, ":phv", long_options, &option_index)) != -1)
  {
    switch (ch)
    {
      case 'p':
        cout << "PDF generation not yet implemented" << endl;
        exit(1);
      case 'h':
        usage();
        exit(0);
      case 'v':
        version();
        exit(0);
      default:
        break;
    }
  }
  argc -= optind;
  argv += optind;
  if ( argc == 0 )
    throw option_error("missing filename");
  _filename = argv[0];
}

void
Options::
version()
{
  cout << "kspec version: 0.1" << endl;
}

void
Options::
usage()
{
  cout << "kspec [options] file" << endl;
  cout << "Options:" << endl;
  cout << "\t-p --pdf     produce keyboard images in PDF format" << endl;
  cout << "\t-v           show version number" << endl;
  cout << "\t-h           output this help text" << endl;
  cout << endl;
}

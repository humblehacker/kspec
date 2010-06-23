#include <iostream>
#include <cstdio>

#include "Options.h"

using namespace std;

int
main (int argc, char *argv[])
{
  try
  {
    Options options(argc, argv);
    cout << "input filename: " << options.filename() << endl;
  }
  catch ( const option_error &e )
  {
    cerr << "Option Error: " << e.what() << endl;
    Options::usage();
    return 1;
  }
  catch(std::exception &e)
  {
    cerr << e.what() << endl;
    Options::usage();
    return 1;
  }
  return 0;
}

extern "C" {
  void yyerror(char *s)
  {
    fprintf(stderr, "error: %s\n", s);
  }
}






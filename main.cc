#include <iostream>
#include <cstdio>

#include "Args.h"

using namespace std;

int
main (int argc, char *argv[])
{
  try
  {
    Args args(argc, argv);
    cout << "input filename: " << args.filename() << endl;
  }
  catch(std::exception &e)
  {
    cerr << e.what() << endl;
  }
  catch(...)
  {
    cerr << "Unknown exception" << endl;
  }
}

extern "C" {
  void yyerror(char *s)
  {
    fprintf(stderr, "error: %s\n", s);
  }
}






#include <stdexcept>

#include "Args.h"

Args::Args(int argc, char *argv[])
{
  if (argc < 2)
    throw std::runtime_error("Invalid number of arguments");
  _filename = argv[1];
}

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>
#include <stdexcept>

class option_error : public std::runtime_error
{
public:
  option_error(const std::string & msg) : std::runtime_error(msg) {}
};

class Options
{
public:
  Options(int argc, char *argv[]);

  const std::string & filename() const { return _filename; }
  bool interactive() const { return _interactive; }
  static void usage();
  static void version();

private:
  std::string _filename;
  bool        _interactive;
};

#endif // __OPTIONS_H__

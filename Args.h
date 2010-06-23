#ifndef __ARGS_H__
#define __ARGS_H__

#include <string>

class Args
{
public:
  Args(int argc, char *argv[]);

  const std::string & filename() const { return _filename; }

private:
  std::string _filename;
};



#endif // __ARGS_H__

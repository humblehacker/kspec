#ifndef __PARSER_CONTEXT_H__
#define __PARSER_CONTEXT_H__

#include <memory>
#include <string>

class Scanner;

struct parser_context
{
  parser_context(const std::string &filename);
  parser_context(const std::string &filename, FILE *fp);
  ~parser_context();

  typedef std::auto_ptr<Scanner> ScannerPtr;

  std::string filename;
  ScannerPtr  scanner;
};

#endif // __PARSER_CONTEXT_H__

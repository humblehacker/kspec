#include "parser_context.h"
#include "scanner.h"

parser_context::
parser_context(const std::string &filename)
: filename(filename), scanner(new Scanner(filename))
{
}

parser_context::
parser_context(const std::string &filename, FILE *fp)
: filename(filename), scanner(new Scanner(fp))
{
}

parser_context::
~parser_context() {}



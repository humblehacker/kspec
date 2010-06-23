
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include <typeinfo>
#include <iomanip>

#include "auxlib.h"
#include "astree.h"
#include "parser.hh"

using std::string;

const char *ASTree::astree_tag = "class ASTree";

ASTree::
ASTree(Token::Type code, const yy::location &loc, const string &lexinfo)
  : the_token(code, loc, lexinfo), tag(astree_tag)
{
   DEBUG ('a', "new [%X]-> %d.%d: %s: (%s)\n",
          (uintptr_t) this, loc.begin.line, loc.begin.column,
          yy::Parser::token_name(code), lexinfo.c_str());
}

ASTree::
~ASTree()
{
   assert(typeid(*this) == typeid(ASTree));
   DEBUG ('a', "delete [%X]-> %d.%d: %s: (%s)\n",
          (uintptr_t) this, the_token.loc.begin.line,
          the_token.loc.begin.column,
          yy::Parser::token_name(the_token.code),
          the_token.name.c_str());
}

unsigned
ASTree::
count() const
{
  unsigned result = 0;
  for (const ASTree *node = first.get(); node; node = node->next.get())
    ++result;
  return result;
}

ASTree::Ptr
ASTree::
adopt(ASTree::Ptr premier, ASTree::Ptr deuxieme, ASTree::Ptr troisieme)
{
  adopt(premier);
  adopt(deuxieme);
  adopt(troisieme);
  return ASTree::Ptr(this);
}

ASTree::Ptr
ASTree::
adopt(ASTree::Ptr left, ASTree::Ptr right)
{
  adopt(left);
  adopt(right);
  return ASTree::Ptr(this);
}

ASTree::Ptr
ASTree::
adopt(ASTree::Ptr child)
{
  assert(child.get() != this);
  if (!child.is_null())
  {
    assert (child.is_correct_type());
    if (last.is_null())
      first = child;
    else
      last->next = child;
    last = child;
    DEBUG ('a', "[%X] (%s) adopting [%X] (%s)\n",
           (uintptr_t) this, the_token.name.c_str(),
           (uintptr_t) child.get(), child->the_token.name.c_str());
  }
  return ASTree::Ptr(this);
}

ASTree::Ptr
ASTree::
absorb(ASTree::Ptr other)
{
  for ( ASTree::Ptr current = other->first; !current.is_null();
        current = current->next )
  {
    adopt(current);
  }
  return Ptr(this);
}

ASTree::Ptr
ASTree::
make_nest()
{
  Ptr nest = ASTree::construct(Token::UNDEF, yy::location(), "");
  nest->adopt(ASTree::Ptr(this));
  return nest;
}




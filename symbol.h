#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>
#include <iosfwd>
#include <algorithm>
#include <inttypes.h>

/*       A T T R I B U T E S
         -------------------
CHAR     Variables and arguments whose base type is char. Functions
         whose base return type is char. Character and string literals.

INT      Variables and arguments whose base type is int. Functions whose
         base return type is int. Integer literals.

VOID     Functions whose base return type is void. Identifiers and
         arguments whose base return type is void. It is an error for a
         variable or argument to be declared as void without a pointer
         attribute.

PTR1, PTR2, PTR3, PTR4 Variables, arguments, and function return types
         declared to have/ return pointers. You must support at least
         four levels of pointers, although an arbitrary number may be
         supported, depending on your implementation. An argument never
         has the ARRAY attribute set, brackets for arguments being the
         same as a pointer. Thus, int a[4], int *a and int a[] all mean
         exactly the same thing in a parameter list.

ARRAY[n] The array attribute for variables and character strings.
         Neither function return types nor arguments may be arrays. The
         number is the dimension given for a variable, and 0 for a
         character string.

ARG.n    A formal parameter declared in a function along with its
         position number. All arguments also have the LVAL attribute
         set.

VAR      Attribute set for variables.

LVAL     All scalar variables have the LVAL attribute set, including
         arguments. In the next assignment, so will any expression
         which is valid as the left operand of the assignment operator.
         Arrays do not have this attribute set, although in the next
         assignment, their elements do.

FUNC#n   All functions have the function attribute turned along, and the
         number of their scope is attached.

*/
namespace Attr
{
  enum
  {
    PTR1  = 1 << 1,
    PTR2  = 1 << 2,
    PTR3  = 1 << 3,
    PTR4  = 1 << 4,
    INT   = 1 << 5,
    CHAR  = 1 << 6,
    VOID  = 1 << 7,
    VAR   = 1 << 8,
    FUNC  = 1 << 9,
    ARG   = 1 << 10,
    LVAL  = 1 << 11,
    ARRAY = 1 << 12,
    TEMP  = 1 << 13
  };
}

class Token;
struct Symbol
{
  Symbol() : scope(0), inner_scope(0), attrs(0), dimension(0),
    token(NULL), next_arg(NULL), queue_link(NULL), parent(NULL) {}
  std::string name;
  unsigned    scope;
  unsigned    inner_scope;
  uint32_t    attrs;
  unsigned    dimension;
  Token      *token;
  Symbol     *next_arg;
  Symbol     *queue_link;
  Symbol     *parent;

  static const uint32_t max_ptr_lvl = 15;
  uint32_t ptr_lvl() const
  {
    return attrs & ~0xFFF0;
  }

  void set_ptr_lvl(uint32_t lvl)
  {
    using namespace Attr;
    attrs &= ~max_ptr_lvl;
    attrs |= std::min<uint32_t>(lvl, max_ptr_lvl);
  }

  void incr_ptr_lvl()
  {
    set_ptr_lvl(ptr_lvl() + 1);
  }
  void decr_ptr_lvl()
  {
    set_ptr_lvl(ptr_lvl() - 1);
  }

  std::string attrs_to_string() const;
  std::string mangled_name() const;
  bool is_pointer() const { return ptr_lvl() > 0; }
  bool is_void_pointer() const { return is_pointer()
                                 && (attrs & Attr::VOID); }
  bool is_non_void_pointer() const { return is_pointer()
                                 && !(attrs & Attr::VOID); }
  bool is_int() const { return attrs & Attr::INT; }
  bool is_lval() const { return attrs & Attr::LVAL; }
};

std::ostream &
operator << (std::ostream &os, const Symbol &symbol);

#endif // __SYMBOL_H__

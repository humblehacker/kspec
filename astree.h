#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <iosfwd>
#include <sys/types.h>
#include <string>
#include <iterator>

#include "shared_ptr.h"
#include "hashtable.h"
#include "location.hh"
#include "token.h"

typedef HashTable<std::string> StringTable;

/*
                      ASTree_iterator
*/

class ASTree;
class ASTree_const_iterator;

class ASTree_iterator
{
public:
  typedef ASTree_iterator         self;
  typedef std::input_iterator_tag iterator_category;
  typedef ptrdiff_t               difference_type;
  typedef ASTree*                 value_type;
  typedef ASTree*                 pointer;
  typedef ASTree&                 reference;

  ASTree_iterator(const value_type node=NULL) : node(node) {}
  ASTree_iterator(const ASTree_iterator &other) : node(other.node) {}
  reference operator*()  const { return *node; }
  pointer   operator->() const { return  node; }

  operator ASTree_const_iterator();

  ASTree_iterator &operator=(const ASTree_iterator &rhs)
    { node = rhs.node; return *this; }

  bool operator==(const ASTree_iterator &rhs) const
    { return node == rhs.node; }
  bool operator!=(const ASTree_iterator &rhs) const
    { return !operator==(rhs); }

  self& operator++();    // prefix
  self  operator++(int); // postfix
  self  operator+(int rhs);

private:
  value_type node;
  friend class ASTree_const_iterator;
};

/*
                   ASTree_const_iterator
*/

class ASTree_const_iterator
{
public:
  typedef ASTree_const_iterator   self;
  typedef std::input_iterator_tag iterator_category;
  typedef ptrdiff_t               difference_type;
  typedef const ASTree*           value_type;
  typedef const ASTree*           pointer;
  typedef const ASTree&           reference;

  ASTree_const_iterator(const value_type node=NULL) : node(node) {}
  reference operator*()  const { return *node; }
  pointer   operator->() const { return  node; }

  bool operator==(const ASTree_const_iterator &rhs) const
    { return node == rhs.node; }
  bool operator!=(const ASTree_const_iterator &rhs) const
    { return !operator==(rhs); }

  self& operator++();    // prefix
  self  operator++(int); // postfix
  self  operator+(int rhs);

private:
  value_type node;
};

inline
ASTree_iterator::
operator ASTree_const_iterator()
{
  return ASTree_const_iterator(node);
}

/*
                          ASTree
*/

class ASTree : public ReferenceCounted
{
public:
  typedef shared_ptr<ASTree>    Ptr;
  typedef ASTree_iterator       iterator;
  typedef ASTree_const_iterator const_iterator;

  static Ptr construct(Token::Type code = Token::UNDEF,
                       const yy::location &loc = yy::location(),
                       const std::string &lexinfo = "µ");
  ~ASTree();

  Ptr adopt(ASTree::Ptr first, ASTree::Ptr second, ASTree::Ptr third);
  Ptr adopt(ASTree::Ptr left,  ASTree::Ptr right);
  Ptr adopt(ASTree::Ptr child);

  Ptr absorb(ASTree::Ptr child);
  Ptr make_nest();

  const Token & token() const { return the_token; }
  Token & token() { return the_token; }

  const std::string  &name()     const { return the_token.name; }
  const yy::location &location() const { return the_token.loc;}
  Token::Type         code()     const { return the_token.code; }
  const Symbol       *sym()      const { return the_token.sym; }

  void set_code(Token::Type code) { the_token.code = code; }

  bool is_leaf() const { return first.is_null(); }

  iterator begin() { return iterator(first.get()); }
  iterator end()   { return iterator(NULL);  }
  const_iterator begin() const { return const_iterator(first.get()); }
  const_iterator end()   const { return const_iterator(NULL);  }

  unsigned count() const;

  template <typename Visitor>
  void apply(Visitor &visit) const;
  template <typename Visitor>
  void postorder(Visitor &visit);

  friend
  std::ostream &operator<<(std::ostream &os, const ASTree &rhs);

private:
  ASTree(Token::Type symbol, const yy::location &loc,
         const std::string &lexinfo);

private:
  Token         the_token;
  ASTree::Ptr   first;   // first child node of this node
  ASTree::Ptr   last;    // last child node of this node
  ASTree::Ptr   next;    // next younger sibling of this node
  const char   *tag;     // tag field to verify class membership

  static const char *astree_tag;
  friend class ASTree_iterator;
  friend class ASTree_const_iterator;
};

inline
ASTree_iterator&
ASTree_iterator::
operator++()
{
  node = node->next.get();
  return *this;
}

inline
ASTree_iterator
ASTree_iterator::
operator++(int)
{
  ASTree_iterator temp(node);
  node = node->next.get();
  return temp;
}

inline
ASTree_iterator
ASTree_iterator::
operator+(int rhs)
{
  value_type next = node;
  while ( rhs-- )
    next = next->next.get();
  return self(next);
}

inline
ASTree_const_iterator&
ASTree_const_iterator::
operator++()
{
  node = node->next.get();
  return *this;
}

inline
ASTree_const_iterator
ASTree_const_iterator::
operator++(int)
{
  self temp(node);
  node = node->next.get();
  return temp;
}

inline
ASTree_const_iterator
ASTree_const_iterator::
operator+(int rhs)
{
  value_type next = node;
  while ( rhs-- )
    next = next->next.get();
  return self(next);
}

inline
ASTree::Ptr
ASTree::
construct(Token::Type code, const yy::location &loc,
          const std::string &lexinfo)
{
  return Ptr(new ASTree(code, loc, lexinfo));
}

inline
std::ostream &
operator<<(std::ostream &os, const ASTree &rhs)
{
  if (rhs.the_token.name == "[")
    os << "'['";
  else
    os << rhs.the_token.name;
  for ( const ASTree *node = rhs.first.get(); node;
        node = node->next.get() )
  {
    os << "[ ";
    os << *node;
    os << " ]";
  }
  return os;
}

// depth-first, left-to-right walk
template <class Visitor>
void
ASTree::
apply(Visitor &visit) const
{
  visit(*this);
  visit.indent();
  for ( ASTree::Ptr node = first; !node.is_null(); node = node->next )
    node->apply(visit);
  visit.undent();
}

template <class Visitor>
void
ASTree::
postorder(Visitor &visit)
{
  for ( ASTree::Ptr node = first; !node.is_null(); node = node->next )
    node->postorder(visit);
  visit(*this);
}


#endif // __ASTREE_H__

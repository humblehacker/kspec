#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "symbol.h"
#include "location.hh"

class Token
{
public:
  enum Type
  {
    UNDEF  = 0,

    ASG    = '=', PER    = '%', COMMA  = ',', LBRACK = '{',
    PLUS   = '+', AMP    = '&', LPAREN = '(', RBRACK = '}',
    MINUS  = '-', GT     = '>', RPAREN = ')',
    PTR    = '*', LT     = '<', LBRACE = '[',
    DIV    = '/', SEMI   = ';', RBRACE = ']',

    ROOT = 258,
    IDENT, LIT_CHAR, LIT_INT, LIT_STRING,
    INT, CHAR, VOID, MULT, FCALL, DEREF,
    RETURN, IF, ELSE, WHILE, EQ, NE, GE, LE
  };

  Token(Type code, const yy::location &loc, const std::string &name)
    : name(name), code(code), loc(loc), sym(NULL) {}

  bool is_type() const;
  bool is_definition() const;
  double serial() const;
  std::string code_name() const;

  std::string   name;
  Type          code;
  yy::location  loc;
  Symbol       *sym;
  bool          internal;
};

inline
bool
Token::
is_definition() const
{
  return (sym && sym->token && sym->token == this);
}

inline
bool
Token::
is_type() const
{
  return code == INT || code == CHAR || code == VOID;
}

inline
double
Token::
serial() const
{
  static const double mult = 1000;  // serial number multiplier
  double result = (loc.begin.line * mult + loc.begin.column) / mult;
  if (internal) result += 1000;
  return result;
}

inline
std::string
Token::
code_name() const
{
  switch (code)
  {
    case UNDEF:      return "UNDEF";
    case ASG:        return "ASG";
    case PER:        return "PER";
    case COMMA:      return "COMMA";
    case LBRACK:     return "LBRACK";
    case PLUS:       return "PLUS";
    case AMP:        return "AMP";
    case LPAREN:     return "LPAREN";
    case RBRACK:     return "RBRACK";
    case MINUS:      return "MINUS";
    case GT:         return "GT";
    case RPAREN:     return "RPAREN";
    case PTR:        return "PTR";
    case LT:         return "LT";
    case LBRACE:     return "LBRACE";
    case DIV:        return "DIV";
    case SEMI:       return "SEMI";
    case RBRACE:     return "RBRACE";
    case ROOT:       return "ROOT";
    case IDENT:      return "IDENT";
    case LIT_CHAR:   return "LIT_CHAR";
    case LIT_INT:    return "LIT_INT";
    case LIT_STRING: return "LIT_STRING";
    case INT:        return "INT";
    case CHAR:       return "CHAR";
    case VOID:       return "VOID";
    case MULT:       return "MULT";
    case FCALL:      return "FCALL";
    case DEREF:      return "DEREF";
    case RETURN:     return "RETURN";
    case IF:         return "IF";
    case ELSE:       return "ELSE";
    case WHILE:      return "WHILE";
    case EQ:         return "EQ";
    case NE:         return "NE";
    case GE:         return "GE";
    case LE:         return "LE";
  }
  return std::string();
}

#endif // __TOKEN_H__

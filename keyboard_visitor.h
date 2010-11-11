#ifndef __KEYBOARD_VISITOR_H__
#define __KEYBOARD_VISITOR_H__

namespace hh
{

class Keyboard;
class Layout;
class KeyMap;
class Key;
class Binding;
class Map;
class Macro;
class Mode;
class Label;

class KeyboardVisitor
{
public:
  virtual ~KeyboardVisitor();
  virtual void visit(const Keyboard & keyboard) = 0;
  virtual void visit(const Layout& keymap) {}
  virtual void visit(const KeyMap & keymap) {}
  virtual void visit(const Key & key) {}
  virtual void visit(const Binding & binding) {}
  virtual void visit(const Map & map) {}
  virtual void visit(const Macro & macro) {}
  virtual void visit(const Mode & mode) {}
  virtual void visit(const Label& mode) {}
};

class KeyboardExternalVisitor
{
public:
  virtual ~KeyboardExternalVisitor();
  virtual void visit(const Keyboard & keyboard) = 0;
  virtual void visit(const Layout& keymap) {}
  virtual void visit(const KeyMap & keymap) {}
  virtual void visit(const Key & key) {}
  virtual void visit(const Binding & binding) {}
  virtual void visit(const Map & map) {}
  virtual void visit(const Macro & macro) {}
  virtual void visit(const Mode & mode) {}
  virtual void visit(const Label& mode) {}
};

} // namespace hh

#endif // __KEYBOARD_VISITOR_H__

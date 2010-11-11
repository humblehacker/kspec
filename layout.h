#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include "keyboard.h"

enum RenderType { RENDER_PDF, RENDER_SVG, RENDER_PNG };

class Layout
{
public:
  Layout(kspec::Keyboard::Ptr kb, RenderType dt);

  void render();

private:
  void render_empty_keyboard();
  void calculate_layout();
  void render(const kspec::Key &key);
  void render(const kspec::KeyMap &keymap, std::wstring &name);
  void render(const kspec::Label &label, const Cairo::Rectangle &rect);

  void write_file();
  void rounded_rect(double x, double y, double width, double height);

private:
  kspec::Keyboard::Ptr _kb;
  RenderType _render_type;
  std::string _filename;
  Cairo::RefPtr<Cairo::Surface> _surface;
  Cairo::RefPtr<Cairo::Context> _cr;
  static const double _scale_factor = 1.8;
  static const double _margin = 0.09;
  Cairo::TextExtents _max_extents;
  double _width;
  double _height;
  std::map<std::wstring, Cairo::Rectangle> _keys;
};

#endif // __LAYOUT_H__

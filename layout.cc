#include <iostream>
#include <iomanip>

#include "utils.h"
#include "layout.h"

using namespace std;

Layout::
Layout(kspec::Keyboard::Ptr kb, RenderType rt)
  : _kb(kb), _render_type(rt), _width(600), _height(400)
{
  switch (rt)
  {
    case RENDER_PDF:
      _filename = "image.pdf";
      _surface = Cairo::PdfSurface::create(_filename, _width, _height);
      break;
    case RENDER_SVG:
      _filename = "image.svg";
      _surface = Cairo::SvgSurface::create(_filename, _width, _height);
      break;
    case RENDER_PNG:
      _filename = "image.png";
      _surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, _width, _height);
      break;
  }
  _cr = Cairo::Context::create(_surface);
}

void
Layout::
calculate_layout()
{
  static double spacing = 0.055;
  double x = 1.0, y = 2.0;

  Cairo::Rectangle rect;
  foreach(const kspec::Layout::RowDef &rowdef, _kb->layout().rows())
  {
    foreach(const kspec::KeyDef::Ptr &keydef, rowdef)
    {
      rect.x = x;
      rect.y = y;
      rect.width = (keydef->width() - spacing) * _scale_factor;
      rect.height = (keydef->height() - spacing) * _scale_factor;
      cout << "(" << setw(3) << wstring_to_string(keydef->ident())
//         << ":" << setw(4) << setprecision(2) << keydef->width()
//         << ":" << setw(4) << rect.width
      << ":" << keydef->is_gap()
      << ") ";
      if (!keydef->is_gap())
        _keys[keydef->ident()] = rect;
      x += rect.width + (spacing * _scale_factor);
    }
    x = 1.0;
    y += 1 * _scale_factor;
    cout << endl;
  }
}

void
Layout::
render_empty_keyboard()
{
  _cr->save();
  _cr->set_source_rgb(0.5, 0.5, 0.5);

  foreach(const kspec::Layout::RowDef &rowdef, _kb->layout().rows())
  {
    foreach(const kspec::KeyDef::Ptr &keydef, rowdef)
    {
      Cairo::Rectangle rect = _keys[keydef->ident()];
      rounded_rect(rect.x, rect.y, rect.width, rect.height);
    }
  }
  _cr->stroke();
  _cr->restore();

}

void
Layout::
render()
{
  calculate_layout();

  _cr->save();
  _cr->select_font_face("Courier", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
  _cr->set_font_size(0.5);
  _cr->set_line_width(0.01);
  _cr->scale(10.0, 10.0);

  _cr->get_text_extents("Xyj", _max_extents);

  foreach(const kspec::KeyMaps::value_type &keymap, _kb->maps())
  {
    render_empty_keyboard();
    std::wstring name;
    render(*keymap.second, name);
    _cr->move_to(1, 1);
    _cr->show_text(wstring_to_string(name));
    _cr->show_page();
  }

  _cr->restore();
  write_file();
}

void
Layout::
render(const kspec::KeyMap &keymap, std::wstring &name)
{
  if (!keymap.base().empty())
  {
    kspec::KeyMaps::const_iterator i = _kb->maps().find(keymap.base());
    assert(i != _kb->maps().end());
    render(*i->second, name);
  }

  if (!name.empty())
    name += L" <- ";
  name += keymap.name();

  foreach(const kspec::Keys::value_type &key, keymap.keys())
  {
    render(key.second);
  }
}

void
Layout::
render(const kspec::Key &key)
{
  const Cairo::Rectangle &rect = _keys[key.location()];
  if (!rect.width || !rect.height)
    return;

#ifdef DEBUG_LAYOUT
  _cr->rectangle(rect.x, rect.y, rect.width, rect.height / 2);
  _cr->rectangle(rect.x, rect.y, rect.width / 2, rect.height);
  _cr->rectangle(rect.x + _margin, rect.y + _margin, rect.width - _margin*2, rect.height - _margin*2);
#endif // DEBUG_LAYOUT

  rounded_rect(rect.x, rect.y, rect.width, rect.height);
  _cr->stroke();
  foreach(const kspec::Bindings::value_type &binding, key.bindings())
  {
    foreach(const kspec::Labels::value_type &label, binding->labels())
    {
      render(label.second, rect);
    }
  }
}

void
Layout::
render(const kspec::Label &label, const Cairo::Rectangle &rect)
{
  string text = wstring_to_string(label.value());
  Cairo::TextExtents extents;
  _cr->get_text_extents(text, extents);

  double x = rect.x;
  double y = rect.y;

  using namespace kspec;
  switch (label.location())
  {
    case Label::top_left:
    case Label::bottom_left:
    case Label::center_left:
      x += _margin;
      break;
    case Label::top_center:
    case Label::center:
    case Label::bottom_center:
      x += rect.width / 2 - extents.width / 2;
      break;
    case Label::top_right:
    case Label::bottom_right:
    case Label::center_right:
      x += rect.width - extents.x_advance - _margin;
      break;
    default:
      wcout << label.loc_as_str() << endl;
  }
  switch (label.location())
  {
    case Label::top_left:
    case Label::top_center:
    case Label::top_right:
      y += _margin - _max_extents.y_bearing;
      break;
    case Label::center_left:
    case Label::center:
    case Label::center_right:
      y += rect.height / 2 - _max_extents.height / 2 - _max_extents.y_bearing;
      break;
    case Label::bottom_left:
    case Label::bottom_center:
    case Label::bottom_right:
      y += rect.height - _margin - (_max_extents.height + _max_extents.y_bearing);
      break;
    default:
      wcout << label.loc_as_str() << endl;
  }

  _cr->save();
  _cr->move_to(x, y);
  _cr->show_text(text);
  _cr->restore();

#ifdef DEBUG_LAYOUT
  _cr->save();
  _cr->set_source_rgb(1, 0, 0);
  _cr->rectangle(x + extents.x_bearing, y + extents.y_bearing, extents.width, extents.height);
  _cr->stroke();
  _cr->restore();

  _cr->save();
  _cr->set_source_rgb(0, 0, 1);
  _cr->rectangle(x, y, extents.x_advance, extents.y_advance + extents.height + extents.y_bearing);
  _cr->stroke();
  _cr->restore();
#endif // DEBUG_LAYOUT
}

void
Layout::
write_file()
{
  if (_render_type == RENDER_PNG)
  {
    _surface->write_to_png(_filename);
  }

  std::cout << "Wrote file \"" << _filename << "\"" << std::endl;
}

void
Layout::
rounded_rect(double x, double y, double width, double height)
{
  double aspect        = 1.0,             /* aspect ratio */
         corner_radius = min(height,width) / 10.0;   /* and corner curvature radius */

  double radius = corner_radius / aspect;
  double degrees = M_PI / 180.0;

  _cr->begin_new_sub_path();
  _cr->arc(x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
  _cr->arc(x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
  _cr->arc(x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  _cr->arc(x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  _cr->close_path();

  _cr->save();
  _cr->set_source_rgb(1, 1, 1);
  _cr->fill_preserve();
  _cr->restore();
}



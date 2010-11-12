#include <iostream>
#include <iomanip>

#include "utils.h"
#include "layout.h"
#include <pangomm/init.h>

//#define DEBUG_LAYOUT

using namespace std;

Layout::
Layout(kspec::Keyboard::Ptr kb, RenderType rt)
  : _kb(kb), _render_type(rt), _width(792), _height(612)
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

  Glib::init();
  Pango::init();
}

void
Layout::
calculate_layout()
{
  static double spacing = 0.055;
  double x = 1.0 * _scale_factor, y = 2.0 * _scale_factor;

  Cairo::Rectangle rect;
  for_each(const kspec::Layout::RowDef &rowdef, _kb->layout().rows())
  {
    for_each(const kspec::KeyDef::Ptr &keydef, rowdef)
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
    x = 1.0 * _scale_factor;
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

  for_each(const kspec::Layout::RowDef &rowdef, _kb->layout().rows())
  {
    for_each(const kspec::KeyDef::Ptr &keydef, rowdef)
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
calculate_max_extents(Pango::Layout &layout)
{
  layout.set_text("j");
  _max_extents = layout.get_pixel_ink_extents();
}

void
Layout::
render()
{
  calculate_layout();

  _cr->save();
  _cr->set_line_width(0.01);
//_cr->scale(10.0, 10.0);

  Glib::RefPtr<Pango::Layout> pango_layout = Pango::Layout::create(_cr);
  Pango::FontDescription desc("Arial Rounded MT Bold");
  desc.set_absolute_size(_font_size * PANGO_SCALE);
  pango_layout->set_font_description(desc);
  pango_layout->set_wrap(Pango::WRAP_WORD_CHAR);

  calculate_max_extents(*pango_layout.operator->());

  for_each(const kspec::KeyMaps::value_type &keymap, _kb->maps())
  {
    render_empty_keyboard();
    std::wstring name;
    render(*keymap.second, name, *pango_layout.operator ->());
    _cr->move_to(1, 1);
    pango_layout->set_width(-1); // turn off wrapping
    pango_layout->set_text(wstring_to_string(name));
    pango_layout->show_in_cairo_context(_cr);
    _cr->show_page();
  }

  _cr->restore();
  write_file();
}

void
Layout::
render(const kspec::KeyMap &keymap, std::wstring &name, Pango::Layout &pango_layout)
{
  if (!keymap.base().empty())
  {
    kspec::KeyMaps::const_iterator i = _kb->maps().find(keymap.base());
    assert(i != _kb->maps().end());
    render(*i->second, name, pango_layout);
  }

  if (!name.empty())
    name += L" <- ";
  name += keymap.name();

  for_each(const kspec::Keys::value_type &key, keymap.keys())
  {
    render(key.second, pango_layout);
  }
}

void
Layout::
render(const kspec::Key &key, Pango::Layout &pango_layout)
{
  Cairo::Rectangle rect = _keys[key.location()];
  if (!rect.width || !rect.height)
    return;

#ifdef DEBUG_LAYOUT
  _cr->rectangle(rect.x, rect.y, rect.width, rect.height / 2);
  _cr->rectangle(rect.x, rect.y, rect.width / 2, rect.height);
  _cr->rectangle(rect.x + _margin, rect.y + _margin, rect.width - _margin*2, rect.height - _margin*2);
#endif // DEBUG_LAYOUT

  rounded_rect(rect.x, rect.y, rect.width, rect.height);
  _cr->stroke();

  rect.x += _margin;
  rect.y += _margin;
  rect.height -= _margin*2;
  rect.width -= _margin*2;

  pango_layout.set_width(rect.width*PANGO_SCALE);   // turn on wrapping
  pango_layout.set_height(rect.height*PANGO_SCALE);

  for_each(const kspec::Bindings::value_type &binding, key.bindings())
  {
    for_each(const kspec::Labels::value_type &label, binding->labels())
    {
      render(label.second, rect, pango_layout);
    }
  }
}

void
Layout::
render(const kspec::Label &label, const Cairo::Rectangle &rect, Pango::Layout &pango_layout)
{
  Pango::Rectangle extents = pango_layout.get_pixel_logical_extents();
  pango_layout.set_text(wstring_to_string(label.value()));

  double y = rect.y;

  using namespace kspec;
  switch (label.location())
  {
    case Label::top_left:
    case Label::bottom_left:
    case Label::center_left:
      pango_layout.set_alignment(Pango::ALIGN_LEFT);
      break;
    case Label::top_center:
    case Label::center:
    case Label::bottom_center:
      pango_layout.set_alignment(Pango::ALIGN_CENTER);
      break;
    case Label::top_right:
    case Label::bottom_right:
    case Label::center_right:
      pango_layout.set_alignment(Pango::ALIGN_RIGHT);
      break;
  }
  switch (label.location())
  {
    case Label::top_left:
    case Label::top_center:
    case Label::top_right:
      y += _margin;
      break;
    case Label::center_left:
    case Label::center:
    case Label::center_right:
      y += rect.height / 2 - _max_extents.get_height() / 2;
      break;
    case Label::bottom_left:
    case Label::bottom_center:
    case Label::bottom_right:
      y += rect.height - _margin - _max_extents.get_height() * pango_layout.get_line_count();
      break;
  }

  _cr->save();
  _cr->move_to(rect.x, y);
  pango_layout.show_in_cairo_context(_cr);
  _cr->restore();

#ifdef DEBUG_LAYOUT
  _cr->save();
  _cr->set_source_rgb(1, 0, 0);
  _cr->rectangle(x,
                 y,
                 extents.get_width(),
                 extents.get_height());
  _cr->stroke();
  _cr->restore();

  _cr->save();
  _cr->set_source_rgb(0, 0, 1);
  _cr->rectangle(x + extents.get_lbearing(),
                 y,
                 extents.get_width() - extents.get_lbearing() - extents.get_rbearing(),
                 extents.get_ascent());
  _cr->stroke();
  _cr->restore();

  _cr->save();
  _cr->set_source_rgb(0, 1, 0);
  _cr->rectangle(x + extents.get_lbearing(),
                 y + extents.get_ascent(),
                 extents.get_width() - extents.get_lbearing() - extents.get_rbearing(),
                 extents.get_descent());
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



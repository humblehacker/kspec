#include <iostream>
#include <iomanip>
#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include "keyboard_visitor.h"
#include "display.h"
#include "utils.h"

using namespace std;

class DisplayKeyboardVisitor : public hh::KeyboardExternalVisitor
{
public:
  DisplayKeyboardVisitor(DisplayType dt);
  ~DisplayKeyboardVisitor();

  virtual void visit(const hh::Keyboard & kb);
  virtual void visit(const hh::Layout & layout);
//virtual void visit(const hh::KeyMap & keymap);
  virtual void visit(const hh::Key & key);
//virtual void visit(const hh::Label & label);
//virtual void visit(const hh::Map & map);
//virtual void visit(const hh::Macro & macro);
//virtual void visit(const hh::Mode & mode);

  void write_file();

private:
  void rounded_rect(double x, double y, double width, double height);

private:
  DisplayType _display_type;
  std::string _filename;
  Cairo::RefPtr<Cairo::Surface> _surface;
  Cairo::RefPtr<Cairo::Context> _cr;
  const int _width;
  const int _height;
};

DisplayKeyboardVisitor::
DisplayKeyboardVisitor(DisplayType dt)
: _display_type(dt), _width(600), _height(400)
{
  switch (dt)
  {
    case DISPLAY_PDF:
      _filename = "image.pdf";
      _surface = Cairo::PdfSurface::create(_filename, _width, _height);
      break;
    case DISPLAY_SVG:
      _filename = "image.svg";
      _surface = Cairo::SvgSurface::create(_filename, _width, _height);
      break;
    case DISPLAY_PNG:
      _filename = "image.png";
      _surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, _width, _height);
      break;
  }
  _cr = Cairo::Context::create(_surface);
}

DisplayKeyboardVisitor::
~DisplayKeyboardVisitor()
{
}

void
DisplayKeyboardVisitor::
rounded_rect(double x, double y, double width, double height)
{
  double aspect        = 1.0,             /* aspect ratio */
         corner_radius = height / 10.0;   /* and corner curvature radius */

  double radius = corner_radius / aspect;
  double degrees = M_PI / 180.0;

  _cr->begin_new_sub_path();
  _cr->arc(x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
  _cr->arc(x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
  _cr->arc(x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  _cr->arc(x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  _cr->close_path();
}

void
DisplayKeyboardVisitor::
visit(const hh::Keyboard & kb)
{
  kb.layout().accept(*this);
  _cr->show_page();
}

void
DisplayKeyboardVisitor::
visit(const hh::Layout & layout)
{
  double x = 0.0, y = 0.0;
  double h, w;

  _cr->save();
  _cr->select_font_face("Courier", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
  _cr->set_font_size(0.6);
  _cr->set_line_width(0.01);    // make the line wider
  _cr->scale(10.0, 10.0);

  double spacing = 0.055;
  const double factor = 1.8;

  foreach(const hh::Layout::RowDef &rowdef, layout.rows())
  {
    foreach(const hh::KeyDef::Ptr &keydef, rowdef)
    {
      w = (keydef->width() - spacing) * factor;
      h = (keydef->height() - spacing) * factor;
      cout << "(" << setw(3) << wstring_to_string(keydef->ident())
           << ":" << setw(4) << setprecision(2) << keydef->width()
           << ":" << setw(4) << w
           << ") ";
      if (!keydef->is_gap())
      {
        rounded_rect(x + (spacing/2*factor), y + (spacing/2*factor), w, h);
        _cr->stroke();
        _cr->move_to(x + 0.2, y + 1);
        _cr->show_text(wstring_to_string(keydef->ident()));
      }
      x += w + (spacing * factor);
    }
    x = 0;
    y += 1 * factor;
    cout << endl;
  }

  _cr->restore();
}

void
DisplayKeyboardVisitor::
visit(const hh::Key & kb)
{
}


void
DisplayKeyboardVisitor::
write_file()
{
  if (_display_type == DISPLAY_PNG)
  {
    _surface->write_to_png(_filename);
  }

  std::cout << "Wrote file \"" << _filename << "\"" << std::endl;
}

void
display(DisplayType dt, hh::Keyboard::Ptr &kb)
{
    DisplayKeyboardVisitor v(dt);
    kb->accept(v);
    v.write_file();
}



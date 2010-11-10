#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <iterator>
#include <cassert>
#include <string>

#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include "Scanner.h"
#include "Parser.h"
#include "utils.h"
#include "generate.h"

using namespace std;

hh::Keyboard::Ptr parse(const wstring &input_filename);
void dump(hh::Keyboard &kb);
bool process_options(int argc, char *argv[], po::options_description &usage,
                     po::variables_map &options);
void produce_image(po::variables_map &options, hh::Keyboard::Ptr &kb);

int
main (int argc, char *argv[])
{
  po::options_description usage("kspec [options] input-file");
  po::variables_map options;
  try
  {
    if (!process_options(argc, argv, usage, options))
      return 0;

    wstring input_filename;
    string_to_wstring(options["input-file"].as<string>(), input_filename);

    hh::Keyboard::Ptr kb;
    kb = parse(input_filename);

    if (options.count("generate-code"))
    {
      generate_code(options, kb);
    }
    else if (options.count("pdf") || options.count("png") || options.count("svg"))
    {
      produce_image(options, kb);
    }
    else if(options.count("program") || options.count("extract"))
    {
      cerr << "Option not yet implemented" << endl;
    }

  }
  catch (const po::error &e)
  {
    cerr << "Option Error: " << e.what() << endl;
    cout << usage << endl;
    return 1;
  }
  catch(exception &e)
  {
    cerr << e.what() << endl;
    cout << usage << endl;
    return 1;
  }
  catch(const char *e)
  {
    cerr << "Unhandled exception: " << e << endl;
    return 1;
  }
  catch(...)
  {
    cerr << "Unhandled exception" << endl;
    return 1;
  }
  return 0;
}

bool
process_options(int argc, char *argv[],
                po::options_description &usage,
                po::variables_map &options)
{
  po::options_description gen_desc("General options");
  gen_desc.add_options()
    ("version,v",        "display the version number")
    ("help,h",           "produce this help text")
  ;
  po::options_description cg_desc("Code generation options");
  cg_desc.add_options()
    ("generate-code,g",  "generate code for keyboard firmware")
    ("output-dir,o",   po::value<string>()->default_value("./generated"),
     "destination for generated code files")
    ("template-dir,t", po::value<string>()->default_value("./templates"),
     "location of code-generation template files")
  ;
  po::options_description dl_desc("Layout display options");
  dl_desc.add_options()
    ("pdf", "produce keyboard images in PDF format")
    ("svg", "produce keyboard images in SVG format")
    ("png", "produce keyboard images in PNG format")
  ;
  po::options_description pg_desc("Programming options");
  pg_desc.add_options()
    ("program", "send keymaps to keyboard")
    ("extract", "read keymaps from keyboard")
  ;

  po::options_description invisible("Invisible options");
  invisible.add_options()
    ("input-file", po::value<string>(), "kspec input file")
    ("debug,d", "produce extra debugging output");

  po::positional_options_description pos_desc;
  pos_desc.add("input-file", 1);

  po::options_description all_desc("");
  all_desc.add(gen_desc).add(cg_desc).add(dl_desc).add(pg_desc).add(invisible);

  usage.add(gen_desc).add(cg_desc).add(dl_desc).add(pg_desc);

  po::command_line_parser parser(argc, argv);
  po::store(parser.options(all_desc).positional(pos_desc).run(), options);
  po::notify(options);

  if (options.count("help"))
  {
    cout << usage;
    return false;
  }

  return true;
}

void
produce_image(po::variables_map &options, hh::Keyboard::Ptr &kb)
{
    std::string filename;
    int width = 600;
    int height = 400;
    Cairo::RefPtr<Cairo::Surface> surface;
    if (options.count("pdf"))
    {
      filename = "image.pdf";
      surface = Cairo::PdfSurface::create(filename, width, height);
    }
    else if (options.count("svg"))
    {
      filename = "image.svg";
      surface = Cairo::SvgSurface::create(filename, width, height);
    }
    else if (options.count("png"))
    {
      filename = "image.png";
      surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    }


    Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

    cr->save(); // save the state of the context
    cr->set_source_rgb(0.86, 0.85, 0.47);
    cr->paint();    // fill image with the color
    cr->restore();  // color is back to black now

    cr->save();
    // draw a border around the image
    cr->set_line_width(20.0);    // make the line wider
    cr->rectangle(0.0, 0.0, cairo_image_surface_get_width(surface->cobj()), height);
    cr->stroke();

    cr->set_source_rgba(0.0, 0.0, 0.0, 0.7);
    // draw a circle in the center of the image
    cr->arc(width / 2.0, height / 2.0,
            height / 4.0, 0.0, 2.0 * M_PI);
    cr->stroke();

    // draw a diagonal line
    cr->move_to(width / 4.0, height / 4.0);
    cr->line_to(width * 3.0 / 4.0, height * 3.0 / 4.0);
    cr->stroke();
    cr->restore();

    cr->show_page();

    if (options.count("png"))
    {
      surface->write_to_png(filename);
    }

    std::cout << "Wrote file \"" << filename << "\"" << std::endl;
}

void
dump(hh::Keyboard &kb)
{
  wcout << "Keyboard: " << kb.ident() << endl;
  wcout << "  Matrix: #rows: "  << kb.matrix().size() << endl;
  wcout << "  Matrix: #cols: "  << kb.matrix().front()->size() << endl;
  wcout << "  RowPins: " << kb.row_pins() << endl;
  wcout << "  ColPins: " << kb.col_pins() << endl;
  wcout << "Keymaps #: " << kb.maps().size() << endl;
  for (hh::KeyMaps::const_iterator i = kb.maps().begin(); i != kb.maps().end(); ++i)
  {
    wcout << "  Keymap: " << i->first << endl;
    wcout << "    base: " << i->second->base() << endl;
    wcout << "    default: " << i->second->default_map() << endl;
    wcout << "    #Keys: " << i->second->keys().size() << endl;
  }
}

hh::Keyboard::Ptr
parse(const wstring &input_filename)
{
  Scanner *scanner = new Scanner(input_filename.c_str());
  Parser  *parser  = new Parser(scanner);
  parser->Parse();
  cerr << parser->errors->count << " errors detected" << endl;
  return parser->kb;
}


#ifdef WIN32
extern "C" {
double __strtod( const char *str, char ** endptr )
{
	return strtod(str, endptr);
}
}
#endif

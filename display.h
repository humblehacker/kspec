#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "keyboard.h"

enum DisplayType { DISPLAY_PDF, DISPLAY_SVG, DISPLAY_PNG };
void display(DisplayType dt, hh::Keyboard::Ptr &kb);

#endif // __DISPLAY_H__

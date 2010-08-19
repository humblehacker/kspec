#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "options.h"
#include "keyboard.h"
#include "usbexception.h"

void program(po::variables_map &options, hh::Keyboard::Ptr &kb);

#endif // __PROGRAM_H__

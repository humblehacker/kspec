
#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "auxlib.h"

static const char *progname = NULL;
static int exitcode = EXIT_SUCCESS;
static char debugflags[ UCHAR_MAX + 1 ];

void set_progname (const char *arg0) {
   progname = (const char *)basename ((char*)arg0);
   //
   // char *slash = NULL;
   // assert (arg0 != NULL);
   // slash = strrchr (arg0, '/');
   // if (slash == NULL) progname = arg0;
   //               else progname = slash + 1;
   //
}

const char *get_progname (void) {
   assert (progname != NULL);
   return progname;
}

static const char *init_dprintf (const char *format) {
   assert (format != NULL);
   xfflush (NULL);
   if (strstr (format, "%:") == format) {
      xfprintf (stderr, "%s: ", get_progname ());
      format += 2;
   };
   return format;
}

void dprintf (const char *format, ...) {
   va_list args;
   format = init_dprintf (format);
   va_start (args, format);
   xvfprintf (stderr, format, args);
   va_end (args);
   xfflush (NULL);
}

void eprintf (const char *format, ...) {
   va_list args;
   va_start (args, format);
   dprintf (format, args);
   va_end (args);
   exitcode = EXIT_FAILURE;
}

void syseprintf (const char *object) {
   eprintf ("%:%s: %s\n", object, strerror (errno));
}

int get_exitcode (void) {
   return exitcode;
}

void set_debugflags (char *flags) {
   char flagset = TRUE;
   for (; *flags; ++flags) {
      if (*flags == '+') {
         xmemset (debugflags, 1, sizeof debugflags);
      }else if (*flags == '-') {
         flagset = FALSE;
      }else{
         debugflags[ (unsigned char) *flags ] = flagset;
      };
   };
}

void __debugprintf (char flag, const char *file, int line,
                    const char *format, ...) {
   va_list args;
   if (! debugflags[ (unsigned char) flag ]) return;
   format = init_dprintf (format);
   xfprintf (stderr, "%s:%d: ", file, line);
   va_start (args, format);
   xvfprintf (stderr, format, args);
   va_end (args);
   xfflush (NULL);
}



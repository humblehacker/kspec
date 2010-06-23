
#ifndef __AUXLIB_H__
#define __AUXLIB_H__

//
// DESCRIPTION
//    Auxiliary library containing miscellaneous useful things.
//

//
// Miscellaneous useful typedefs.
//

#ifndef __cplusplus
  typedef enum {FALSE = 0, TRUE = 1} bool;
#else
  #define FALSE false
  #define TRUE  true
#endif

//
// Error message and exit code utility.
//

void set_progname (const char *arg0);
   //
   // Sets the program name for error messages printed by this
   // library.  Usually just called with argv[0].  Extracts the
   // basename of the filename.
   //

const char *get_progname (void);
   //
   // Returns a read-only value previously stored by `set_progname'.
   //

void dprintf (const char *format, ...);
   //
   // Print an message to stderr according to the printf format
   // specified.  Usually called for debug output.
   // Precedes the message by the program name if the format
   // begins with the characters `%:'.
   //

void eprintf (const char *format, ...);
   //
   // Calls dprintf to print a message to stderr, then sets
   // the exit code to indicate failure.  Usually called to
   // report errors.
   //

void syseprintf (const char *object);
   //
   // Print a message resulting from a bad system call.  The
   // object is the name of the object causing the problem and
   // the reason is taken from the external variabl errno.
   //

int get_exitcode (void);
   //
   // Returns the exit code set by the above functions.  Default
   // is EXIT_SUCCESS, but any call to eprintf causes it to be
   // set to EXIT_FAILURE.  The last statement in function `main'
   // should be:  ``return get_exitcode();''.
   //

//
// Debugging utility.
//

void set_debugflags (char *flags);
   //
   // Sets the debugflags in the give string.  When `DEBUG' is
   // called with one of those flags as the first argument,
   // output is generated.  Special cases:
   // '+' turns on all of the flags
   // '-' turns off all flags following the '-'.
   //

#ifdef NDEBUG
#define DEBUG(FLAG,...) // DEBUG (FLAG, __VA_ARGS__)
#else
#define DEBUG(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __VA_ARGS__)
void __debugprintf (char flag, const char *file, int line,
                    const char *format, ...);
#endif
   //
   // Macro: DEBUG (flag, format, fmtargs)
   //
   // If the flag is turned on, a debug message is printed to stderr.
   // This works like `dprintf', except that the message is preceded
   // by the file and line number, and the exit code is not set.
   // Generally, the macro `DEBUG' should be used.  `__debugprintf'
   // should not be called directly.
   //

//
// Redefinition of a few functions to keep lint from whining about
// ``function returns value which is always ignored''
//

#define xfflush   (void) fflush
#define xfprintf  (void) fprintf
#define xmemset   (void) memset
#define xprintf   (void) printf
#define xsprintf  (void) sprintf
#define xvfprintf (void) vfprintf

#endif


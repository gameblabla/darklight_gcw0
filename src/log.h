// Log (.H)
// --------

#ifndef LOG_H_
#define LOG_H_

#include <stdlib.h>
#include <stdio.h>



// Bounds Checking:
// ----------------

// If BOUNDS_CHECKING is defined then the SET macro traps any bound overruns.
// If it is not defined then the SET macro is converted to a standard assignment
// with no speed loss.

// This macro can be used for one dimensional Structure & Array writes.

#define BOUNDS_CHECKING

#ifdef BOUNDS_CHECKING
#define SET(array, item)  (item < 0 || item >= (sizeof(array) /  sizeof(array[0])) \
? overrun_error(#array, item, __LINE__, __FILE__), array[0] : array[item])
#else
#define SET(array, item) array[item]
#endif

int overrun_error(char *struc, int pos, int line, char *file);

// Logging functions:
// ------------------

int open_log(char *filename);
int write_to_log(char *format, ...);
int write_to_log_no_cr(char *format, ...);
int record_error(char *format, ...);
int close_log(void);

int write_log_header(void);

#endif

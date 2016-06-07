#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <stdarg.h>

/*
 * Very simple logger that mimicks syntax of stdio
 */

// Log levels
typedef enum {
	ALL,     // Everything
	TRACE,   // Excessive debugging
	DEBUG,   // Verbose
	INFO,    // Information
	WARN, 	 // Potential problem
	ERROR,   // Something went wrong
	FATAL    // Crash
} LogLevel;

// Opens a log file to write to
void lopen(const char *filename);

// Closes log file if one is open
void lclose();

// Log with formatting, syntax like fprintf
void lprintf(int level, const char *format, ...);

#endif

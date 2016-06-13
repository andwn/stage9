#include "log.h"

#include <stdio.h>

int logLevel = TRACE;
FILE *logfile;

char *levelStr[7] = {
	"[ALL]",
	"[TRACE]",
	"[DEBUG]",
	"[INFO]",
	"[WARN]",
	"[ERROR]",
	"[FATAL]"
};

void lopen(const char *filename) {
	logfile = fopen(filename, "w");
	if(logfile == NULL) {
		lprintf(ERROR, "Unable to create log \"%s\".\n", filename);
	}
}

void lclose() {
	if(logfile == NULL) {
		lprintf(WARN, "Called lclose() but no log file is open.\n");
	} else {
		lprintf(TRACE, "Closing log file.\n");
		fclose(logfile);
	}
}

void lprintf(int level, const char *format, ...) {
	if(logLevel > level) return;
	// Print to file
	if(logfile != NULL) {
		fprintf(logfile, "%s ", levelStr[level]);
		va_list args;
		va_start(args, format);
		vfprintf(logfile, format, args);
		va_end(args);
		fprintf(logfile, "\n");
	}
	// Print to console output
	printf("%s ", levelStr[level]);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}

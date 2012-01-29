// Utility functions
// (c) 2001 Andrew de Quincey

#include "mytypes.h"
#include <stdio.h>
#include <stdarg.h>
#include "util.h"

/**
 * Report an error
 */
void error(char *fmt, ...) {
  va_list argp;
  fprintf(stderr, "error: ");
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  fprintf(stderr, "\n");
  exit(1);
}


/**
 * Load a DOS date and time stamp
 *
 * @param dateTime Place to put data
 * @param date Raw DOS date value
 * @param time Raw DOS time value
 */
void loadDosDateTime(DosDateTime* dateTime, 
                     u_int16_t date, u_int16_t time) {
  dateTime->day = date & 0x1f;
  dateTime->month = (date & 0x1e0) >> 5;
  dateTime->year = ((date & 0xfe00) >> 9) + 1980;
  dateTime->secs = (time & 0x1f) * 2;
  dateTime->mins = (time & 0x7e0) >> 5;
  dateTime->hours = (time & 0xf800) >> 11;
}



/**
 * Format a DOSDateTime for printing
 *
 * @param dateTime DosDateTime to format
 *
 * @return Formatted string
 */
char* formatDosDate(DosDateTime* dateTime) {
  static char formatDosDateSTORE[256];
  
  sprintf(formatDosDateSTORE, "%02i:%02i:%02i-%i/%i/%i",
          dateTime->hours, dateTime->mins, dateTime->secs, 
          dateTime->day, dateTime->month, dateTime->year);
  return formatDosDateSTORE;
}

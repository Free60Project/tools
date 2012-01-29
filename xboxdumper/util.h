// Utility functions
// (c) 2001 Andrew de Quincey


#ifndef UTIL_H
#define UTIL_H 1

#include <sys/types.h>

/**
 * Structure to contain a DOS date and timestamp
 */
typedef struct {
  int year;
  int month;
  int day;
  int hours;
  int mins;
  int secs;
} DosDateTime;

/**
 * Report an error
 */
void error(char *fmt, ...);

/**
 * Load a DOS date and time stamp
 *
 * @param dateTime Place to put data
 * @param date Raw DOS date value
 * @param time Raw DOS time value
 */
void loadDosDateTime(DosDateTime* dateTime, 
                     u_int16_t date, u_int16_t time);



/**
 * Format a DOSDateTime for printing
 *
 * @param dateTime DosDateTime to format
 *
 * @return Formatted string
 */
char* formatDosDate(DosDateTime* dateTime);

#endif


#include "RunTimeLog.h"

#include <fstream>
#include <ctime>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define BUFFER_SIZE 1000
char* ctime () 
{
	time_t timer;
	time (&timer);
	char* s_time = ctime (&timer);
	char* c = strrchr (s_time, '\n');
	if (NULL != c)
		*c = 0;
	return s_time;
}


RunTimeLog::RunTimeLog (const char* logfilename)
	: log_filename (logfilename)
	, open_success (false)
{
	log_file.open (log_filename, std::ios::app);
	assert (log_file.is_open ());	
	open_success = log_file.is_open ();
	log_file << std::endl <<  "<start time = \"" << ctime() << ">" << std::endl; 
}


RunTimeLog::~RunTimeLog ()
{
	if (log_file.is_open ()) {
	  log_file << "</end time = \"" << ctime() << ">" << std::endl << std::endl; 
		log_file.close ();
	}
	open_success = false;
}


bool RunTimeLog::write_log (LogType type, const char* logtext, ...)
{
	if (!open_success) {
	 log_file.open (log_filename);
	 if (log_file.fail ()) {
		 return false;
	 }
	} 
	log_file << "<" << type_to_str (type) << " time = \"" << ctime() << "\">"; 
	va_list args;
	char buffer[BUFFER_SIZE] = {0};
	va_start (args, logtext);
	int pos = 0;
	int buffer_pos = 0;
	while (logtext[pos]) {
		if ('%' == logtext[pos]) {
			pos++;
			switch (logtext[pos]) {
				case 's':{
					unsigned char* chptr = va_arg (args, unsigned char *);
					//printf ("in %s\n", chptr);
					int i = 0;
					while (chptr[i] && buffer_pos < BUFFER_SIZE) {
						//printf ("%c\n", chptr[i]);
						buffer[buffer_pos++] = chptr[i++];
					}
					pos++;
					break;
				}
				case 'd':{
					buffer_pos +=  sprintf (&buffer[buffer_pos], "%d", va_arg (args, int));
					pos++;
					break;
				}
				default:
					break;
			}
		}
		else if (buffer_pos < BUFFER_SIZE) {
			buffer[buffer_pos++] = logtext[pos++];
		}
		else {
			break;
		}
	}
	log_file.write (buffer, strlen(buffer));
	log_file << "</" << type_to_str (type) << ">" << std::endl;
	return true;
}




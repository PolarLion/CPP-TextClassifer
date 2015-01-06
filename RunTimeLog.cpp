#include "RunTimeLog.h"

#include <fstream>
#include <ctime>
#include <assert.h>
#include <string.h>


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
	log_file << "<start time = \"" << ctime() << ">" << std::endl; 
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
	log_file.write (logtext, strlen(logtext));
	log_file << "</" << type_to_str (type) << ">" << std::endl;
	return true;
}




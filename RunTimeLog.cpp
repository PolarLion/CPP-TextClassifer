#include "RunTimeLog.h"

#include <fstream>
#include <ctime>
#include <string>
#include <string.h>

RunTimeLog::RunTimeLog (const char* logfilename)
	: log_filename (logfilename)
	, open_sucess (false)
{
	log_file.open (log_filename);
	open_success = log_file.is_open ();
}


RunTimeLog::~RunTimeLog ()
{
	if (log_file.is_open ()) {
		log_file.close ();
	}
}


bool RunTimeLog::write_log (const char* logtext, LogType type)
{
	if (!open_success) {
	 log_file.open (log_filename);
	 if (log_file.fail ()) {
		 return false;
	 }
	} 
	log_file.write (logtext, strlen(logtext));
	return true;
}


bool RunTimeLog::clear_log ()
{
	return true;
}


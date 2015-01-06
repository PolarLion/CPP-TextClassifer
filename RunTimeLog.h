#pragma once

#include <fstream>



class RunTimeLog 
{
private:
	const char* log_filename;
	std::ofstream log_file;
	bool open_success;
public:
	enum LogType {
		LOGTYPE_ERROR = 0,
		LOGTYPE_WARNING = 1,
		LOGTYPE_NORMAL = 2
	};
public:
	RunTimeLog (const char* logfilename);
	~RunTimeLog ();

	bool write_log (const char* logtext, LogType type);
	bool clear_log ();
	bool is_open () const {
		return open_success;
	}
};

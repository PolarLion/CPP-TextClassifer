#pragma once

#include <fstream>


class RunTimeLog 
{
public:
	enum LogType {
		LOGTYPE_ERROR = 0,
		LOGTYPE_WARNING = 1,
		LOGTYPE_NORMAL = 2
	};
private:
	const char* log_filename;
	std::ofstream log_file;
	bool open_success;

	const char* type_to_str (LogType type) const {
		if (LOGTYPE_ERROR == type) {
			return "Error";
		}
		else if (LOGTYPE_NORMAL == type) {
			return "Normal";
		}
		else if (LOGTYPE_WARNING == type) {
			return "Warning";
		}
		return "";
	}
public:
	RunTimeLog (const char* logfilename);
	~RunTimeLog ();

	bool write_log (const char* logtext, LogType type);
	bool is_open () const {
		return open_success;
	}
};

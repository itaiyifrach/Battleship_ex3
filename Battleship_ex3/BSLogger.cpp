#include "BSLogger.h"

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const char* BSLogger::currentDateTime() {
	time_t     now = time(nullptr);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

void BSLogger::loggerPrint(const char* msg)
{
	logger << currentDateTime() << ":\t";
	logger << msg << endl;
}
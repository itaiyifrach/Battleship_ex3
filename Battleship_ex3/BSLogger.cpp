#include "BSLogger.h"
ofstream logger;


void BSLogger::initLogger(string& basePath)
{
	string loggerName = LOGGER_NAME;

	// opening the log file
	logger.open(basePath + loggerName, ios::out | ios::trunc);

	// opening line of the logger
	loggerPrint("Starting the program...");

}
void BSLogger::closeLogger()
{
	// closing the log file
	logger.close();
}

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

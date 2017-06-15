#include "BSLogger.h"
ofstream logger;


void BSLogger::initLogger(string& basePath)
{
	string loggerName = LOGGER_NAME;

	// opening the log file
	logger.open(basePath + "\\" + loggerName, ios::out | ios::trunc);

	// opening line of the logger
	loggerPrintInfo("Starting the program...");

}
void BSLogger::closeLogger()
{
	loggerPrintInfo("Closing the program...");
	// closing the log file
	logger.close();
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
string BSLogger::currentDateTime() {
	time_t     now = time(nullptr);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%d-%m-%Y  %X", &tstruct);

	return buf;
}

void BSLogger::loggerPrintError(const string& msg)
{
	logger << currentDateTime().c_str() << ":\t" << ERROR;
	logger << msg.c_str() << endl;
}

void BSLogger::loggerPrintInfo(const string& msg)
{
	logger << currentDateTime().c_str() << ":\t" << INFO;
	logger << msg.c_str() << endl;
}

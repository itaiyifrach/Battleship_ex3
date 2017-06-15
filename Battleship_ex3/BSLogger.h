#pragma once
#include <fstream>
#include <ctime>
#include <memory>

using namespace std;

#define LOGGER_NAME "game.log"

// Global variable holding the logger
ofstream logger;

class BSLogger
{
public:
	BSLogger(string& basePath)
	{
		string loggerName = LOGGER_NAME;
		
		// opening the log file
		logger.open(basePath + loggerName, ios::out | ios::trunc);

		// opening line of the logger
		loggerPrint("Starting the program...");
	};

	~BSLogger()
	{
		// closing the log file
		logger.close();
	};

	static void loggerPrint(const char* msg);
	static const char* currentDateTime();
};
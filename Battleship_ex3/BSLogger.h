#pragma once
#include <fstream>
#include <string>
#include <ctime>
#include <memory>

using namespace std;

#define LOGGER_NAME "game.log"
#define LOGGER_ERROR "<ERROR> "
#define LOGGER_INFO "<INFO> "

// Global variable holding the logger
extern ofstream logger;

class BSLogger
{
public:
	static void initLogger(string& basePath);
	static void closeLogger();
	static void loggerPrintError(const string& msg);
	static void loggerPrintInfo(const string& msg);
	static string currentDateTime();
};
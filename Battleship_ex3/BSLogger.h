#pragma once
#include <fstream>
#include <string>
#include <ctime>
#include <memory>

using namespace std;

#define LOGGER_NAME "game.log"

// Global variable holding the logger
extern ofstream logger;

class BSLogger
{
public:
	static void initLogger(string& basePath);
	static void closeLogger();
	static void loggerPrint(const char* msg);
	static const char* currentDateTime();
};
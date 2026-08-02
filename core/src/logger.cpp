#include "logger.h"

std::ofstream logFile;

void initLogger() {
	logFile.open("log.txt", std::ios::out);
}

void closeLogger() {
	logFile.close();
}
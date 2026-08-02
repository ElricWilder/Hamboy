#pragma once
#include <fstream>

extern std::ofstream logFile;

void initLogger();
void closeLogger();
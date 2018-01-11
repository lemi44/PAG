#include "Logger.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#ifdef _DEBUG
#define LOG_DEFAULT_LEVEL debug
#else
#define LOG_DEFAULT_LEVEL info
#endif
std::string Logger::filename = "";
level Logger::current_level = LOG_DEFAULT_LEVEL;
std::ofstream Logger::log_file;

Logger::~Logger()
{
	if(log_file.is_open())
	log_file << std::flush;
	log_file.close();
}

void Logger::log(const std::string text, const level level)
{
	if (filename.empty())
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm tm;
		localtime_s(&tm, &t);
		char mbstr[100];

		if (std::strftime(mbstr, sizeof(mbstr), "log_%Y-%m-%d_%H%M%S.txt", &tm)) {
			filename = std::string(mbstr);
			log_file.open(filename, std::ios_base::out | std::ios_base::app);
		}
	}
	std::string level_str;
	switch (level)
	{
	case error:
		level_str = "ERROR: ";
		break;
	case warning:
		level_str = "WARNING: ";
		break;
	case info:
		level_str = "INFO: ";
		break;
	case debug:
		level_str = "DEBUG: ";
		break;
	}
	std::string line;
	{
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		tm tm;
		localtime_s(&tm, &t);
		char mbstr[100];
		std::string timestamp;
		if (std::strftime(mbstr, sizeof(mbstr), "%F %T", &tm)) {
			timestamp = std::string(mbstr);
		}
		std::ostringstream oss;
		oss << "[" << timestamp << "] " << level_str << text << "\n";
		line = oss.str();
	}
	if (log_file.is_open())
	{
		log_file << line;
	}
	if (level <= current_level)
		if (level <= warning)
			std::cerr << line;
		else
			std::cout << line;
}


void Logger::setCurrentLevel(const level level)
{
	current_level = level;
}
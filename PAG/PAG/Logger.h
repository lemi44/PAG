#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <cstdio>
#include <glad/glad.h>

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

enum level
{
	error = 0,
	warning = 1,
	info = 2,
	debug = 3
};

class Logger
{
	static std::string filename;
	static level current_level;
	static std::ofstream log_file;
public:
	~Logger();
	static void log(const std::string text, level level);
	static void logError(const std::string text) { log(text, error); }
	static void logWarning(const std::string text) { log(text, warning); }
	static void logInfo(const std::string text) { log(text, info); }
	static void logDebug(const std::string text) { log(text, debug); }
	static void setCurrentLevel(level level);
};


#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <sstream>

enum class LogLevel {
    INFO,
    WARNING,
    ERRORS,
    SUCCESS,
    DEBUG
};

class Logger {
public:
    static void Initialize();
    static void Shutdown();

    static void Info(const std::string& message, const char* file = __FILE__, int line = __LINE__);
    static void Warning(const std::string& message, const char* file = __FILE__, int line = __LINE__);
    static void Error(const std::string& message, const char* file = __FILE__, int line = __LINE__);
    static void Success(const std::string& message, const char* file = __FILE__, int line = __LINE__);
    static void Debug(const std::string& message, const char* file = __FILE__, int line = __LINE__);

private:
    static void Log(LogLevel level, const std::string& message, const char* file, int line);
    static void SetConsoleColor(LogLevel level);
    static std::string GetLevelPrefix(LogLevel level);
    static std::string ExtractFileName(const char* filePath);

    static bool m_initialized;
    static HANDLE m_consoleHandle;
    static std::ofstream m_logFile;
};

// Macros
#define LOG_INFO(msg) Logger::Info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::Warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::Error(msg, __FILE__, __LINE__)
#define LOG_SUCCESS(msg) Logger::Success(msg, __FILE__, __LINE__)

#ifdef _DEBUG
#define LOG_DEBUG(msg) Logger::Debug(msg, __FILE__, __LINE__)
#else
#define LOG_DEBUG(msg)
#endif
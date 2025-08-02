#include "Logger.h"
#include "core/globals.h"
#include <filesystem>
#include <ctime>
#include <iomanip>

bool Logger::m_initialized = false;
HANDLE Logger::m_consoleHandle = nullptr;
std::ofstream Logger::m_logFile;

void Logger::Initialize() {
    if (m_initialized) return;

#ifdef _DEBUG
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    SetConsoleTitleA("SpeX-Core Debug Console");

    m_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_consoleHandle, &csbi);
    COORD newSize;
    newSize.X = 120;
    newSize.Y = 3000;
    SetConsoleScreenBufferSize(m_consoleHandle, newSize);

    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = 119;
    windowSize.Bottom = 29;
    SetConsoleWindowInfo(m_consoleHandle, TRUE, &windowSize);
#endif

    std::string documentsPath = std::getenv("USERPROFILE");
    documentsPath += "\\Documents\\SpeXCore";
    std::filesystem::create_directories(documentsPath);
    
    std::string logPath = documentsPath + "\\Debug.log";
    m_logFile.open(logPath, std::ios::app);

    m_initialized = true;

    LOG_SUCCESS("Logger initialized successfully");
    LOG_INFO("SpeX-Core");
    LOG_INFO("Build: " __DATE__ " " __TIME__);
}

void Logger::Shutdown() {
    if (!m_initialized) return;

    LOG_INFO("Shutting down logger...");

    if (m_logFile.is_open()) {
        m_logFile.close();
    }

#ifdef _DEBUG
    if (m_consoleHandle) {
        FreeConsole();
    }
#endif

    m_initialized = false;
}

void Logger::Info(const std::string& message, const char* file, int line) {
    Log(LogLevel::INFO, message, file, line);
}

void Logger::Warning(const std::string& message, const char* file, int line) {
    Log(LogLevel::WARNING, message, file, line);
}

void Logger::Error(const std::string& message, const char* file, int line) {
    Log(LogLevel::ERRORS, message, file, line);
}

void Logger::Success(const std::string& message, const char* file, int line) {
    Log(LogLevel::SUCCESS, message, file, line);
}

void Logger::Debug(const std::string& message, const char* file, int line) {
#ifdef _DEBUG
    Log(LogLevel::DEBUG, message, file, line);
#endif
}

void Logger::Log(LogLevel level, const std::string& message, const char* file, int line) {
    if (!m_initialized) return;

    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);

    std::stringstream ss;
    ss << std::put_time(&tm, "%H:%M:%S");
    std::string timeStr = ss.str();

    std::string fileName = ExtractFileName(file);
    std::string prefix = GetLevelPrefix(level);

    std::stringstream logMessage;
    logMessage << "[" << timeStr << "] " << prefix << " " << message
        << " (" << fileName << ":" << line << ")";

    std::string fullMessage = logMessage.str();

#ifdef _DEBUG
    if (m_consoleHandle) {
        SetConsoleColor(level);
        std::cout << fullMessage << std::endl;
        SetConsoleTextAttribute(m_consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
#endif

    if (m_logFile.is_open()) {
        m_logFile << fullMessage << std::endl;
        m_logFile.flush();
    }
}

void Logger::SetConsoleColor(LogLevel level) {
    if (!m_consoleHandle) return;

    WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    switch (level) {
    case LogLevel::INFO:
        color = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    case LogLevel::WARNING:
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case LogLevel::ERRORS:
        color = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
    case LogLevel::SUCCESS:
        color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
    case LogLevel::DEBUG:
        color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
    }

    SetConsoleTextAttribute(m_consoleHandle, color);
}

std::string Logger::GetLevelPrefix(LogLevel level) {
    switch (level) {
    case LogLevel::INFO:    return "[INFO]";
    case LogLevel::WARNING: return "[WARN]";
    case LogLevel::ERRORS:   return "[ERROR]";
    case LogLevel::SUCCESS: return "[+]";
    case LogLevel::DEBUG:   return "[DEBUG]";
    default:                return "[LOG]";
    }
}

std::string Logger::ExtractFileName(const char* filePath) {
    std::string path(filePath);
    size_t pos = path.find_last_of("\\/");
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return path;
}
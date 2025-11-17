#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

// Log level definition
enum class LogLevel {
  DEBUG,
  INFO,
  WARN,
  ERROR
};

// Thread-safe singleton logger
class Logger {
 public:
  // Get singleton instance
  static Logger &instance();

  // Delete copy constructor and assignment
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // Initialize logger with file path and minimum level
  void init(const std::string &logFilename, LogLevel level = LogLevel::INFO);

  // Log debug message
  void debug(const std::string &message);

  // Log info message
  void info(const std::string &message);

  // Log warning message
  void warn(const std::string &message);

  // Log error message
  void error(const std::string &message);

 private:
  // Private constructor
  Logger() = default;

  // Destructor (handled by singleton lifecycle)
  ~Logger();

  // Core logging function
  void log(LogLevel level, const std::string &message);

  // Convert LogLevel to string
  std::string levelToString(LogLevel level);

  std::ofstream logFile_;                // Output file stream
  LogLevel minLevel_ = LogLevel::INFO;   // Minimum log level
  std::mutex mutex_;                     // Thread-safety lock
  bool initialized_ = false;             // Initialization flag
};

#endif   // LOGGER_H
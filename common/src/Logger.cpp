#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// Get singleton instance
Logger &Logger::instance() {
  static Logger instance;
  return instance;
}

// Destructor (close log file)
Logger::~Logger() {
  if (logFile_.is_open()) {
    logFile_.close();
  }
}

// Initialize logger with file path and level
void Logger::init(const string &logFilename, LogLevel level) {
  if (initialized_) return;

  minLevel_ = level;
  logFile_.open(logFilename, ios::out | ios::app);
  if (!logFile_.is_open()) {
    cerr << "CRITICAL ERROR: Could not open log file: " << logFilename << endl;
  }

  initialized_ = true;
  info("Logger system initialized.");
}

// Log debug message
void Logger::debug(const string &message) {
  log(LogLevel::DEBUG, message);
}

// Log info message
void Logger::info(const string &message) {
  log(LogLevel::INFO, message);
}

// Log warning message
void Logger::warn(const string &message) {
  log(LogLevel::WARN, message);
}

// Log error message
void Logger::error(const string &message) {
  log(LogLevel::ERROR, message);
}

// Core logging function
void Logger::log(LogLevel level, const string &message) {
  if (!initialized_ || level < minLevel_) return;

  // Generate timestamp string
  auto now = chrono::system_clock::now();
  auto inTimeT = chrono::system_clock::to_time_t(now);
  auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

  stringstream ts;
  ts << put_time(localtime(&inTimeT), "%Y-%m-%d %H:%M:%S")
     << '.' << setfill('0') << setw(3) << ms.count();

  stringstream ss;
  ss << "[" << ts.str() << "] "
     << "[" << setw(5) << left << levelToString(level) << "] "
     << message;

  string logLine = ss.str();

  lock_guard<mutex> lock(mutex_);

  // Output to console
  if (level == LogLevel::ERROR)
    cerr << logLine << endl;
  else
    cout << logLine << endl;

  // Write to file
  if (logFile_.is_open()) {
    logFile_ << logLine << endl;
  }
}

// Convert LogLevel to string
string Logger::levelToString(LogLevel level) {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}
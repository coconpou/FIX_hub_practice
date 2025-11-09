#ifndef LOGGER_H_
#define LOGGER_H_

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

// FIX message alias
using FixMessage = std::map<int, std::string>;

class Logger {
 private:
  std::string logFilename_;                                          // Log file name
  std::ofstream logFile_;                                            // Log file stream
  std::mutex logMutex_;                                              // Protects log writing
  std::map<std::string, std::vector<FixMessage>> pendingMessages_;   // Offline FIX messages
  std::mutex queueMutex_;                                            // Protects pending queue

  // Get timestamp in format "YYYY-MM-DD HH:MM:SS"
  std::string GetTimestamp() const;

  // Write a log entry to file
  void WriteLog(const std::string &level, const std::string &logMessage);

 public:
  // Constructor
  Logger(const std::string &logFilename = "fix_hub.log");

  // Destructor
  ~Logger();

  // Log validation result
  void LogValidationResult(const std::string &sessionID, bool success, const std::string &reason);

  // Log info message
  void LogInfo(const std::string &message);

  // Log error message
  void LogError(const std::string &message);

  // Queue a FIX message for offline target
  void QueueMessageForOfflineTarget(const std::string &targetCompID, const FixMessage &msg);

  // Check if target has pending messages
  bool HasPendingMessages(const std::string &targetCompID);

  // Retrieve pending messages for target
  std::vector<FixMessage> RetrievePendingMessages(const std::string &targetCompID);

  // Print all log file contents
  void PrintLogFileContents();
};

#endif   // LOGGER_H_
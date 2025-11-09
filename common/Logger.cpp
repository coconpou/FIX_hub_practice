#include "Logger.h"

using namespace std;

// Constructor
Logger::Logger(const string &logFilename) : logFilename_(logFilename) {
  logFile_.open(logFilename_, ios::out | ios::app);
  if (!logFile_.is_open()) {
    cerr << "CRITICAL ERROR: Could not open log file: " << logFilename_ << endl;
  }
  LogInfo("Logger system initialized.");
}

// Destructor
Logger::~Logger() {
  LogInfo("Logger system shutting down.");
  if (logFile_.is_open()) logFile_.close();
}

// Get current timestamp "YYYY-MM-DD HH:MM:SS.mmm"
string Logger::GetTimestamp() const {
  auto now = chrono::system_clock::now();
  auto in_time_t = chrono::system_clock::to_time_t(now);
  auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

  stringstream ss;
  ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S")
     << '.' << setfill('0') << setw(3) << ms.count();
  return ss.str();
}

// Write formatted log line to console and file
void Logger::WriteLog(const string &level, const string &logMessage) {
  lock_guard<mutex> lock(logMutex_);
  stringstream ss;
  ss << "[" << GetTimestamp() << "] "
     << "[" << setw(8) << left << level << "] "
     << logMessage;

  string logLine = ss.str();

  if (level == "ERROR")
    cerr << logLine << endl;
  else
    cout << logLine << endl;

  if (logFile_.is_open()) logFile_ << logLine << endl;
}

// Log validation result
void Logger::LogValidationResult(const string &sessionID, bool success, const string &reason) {
  stringstream ss;
  ss << "Session [" << sessionID << "] "
     << (success ? "Validation SUCCESS" : "Validation FAILED")
     << ". Reason: " << reason;
  WriteLog("VALIDATE", ss.str());
}

// Log info message
void Logger::LogInfo(const string &message) {
  WriteLog("INFO", message);
}

// Log error message
void Logger::LogError(const string &message) {
  WriteLog("ERROR", message);
}

// Queue FIX message for offline target
void Logger::QueueMessageForOfflineTarget(const string &targetCompID, const FixMessage &msg) {
  lock_guard<mutex> lock(queueMutex_);
  pendingMessages_[targetCompID].push_back(msg);

  stringstream ss;
  ss << "Target [" << targetCompID << "] is offline. Queuing message.";
  LogInfo(ss.str());
}

// Check if target has pending messages
bool Logger::HasPendingMessages(const string &targetCompID) {
  lock_guard<mutex> lock(queueMutex_);
  auto it = pendingMessages_.find(targetCompID);
  return (it != pendingMessages_.end() && !it->second.empty());
}

// Retrieve pending messages for target
vector<FixMessage> Logger::RetrievePendingMessages(const string &targetCompID) {
  lock_guard<mutex> lock(queueMutex_);
  auto it = pendingMessages_.find(targetCompID);

  if (it != pendingMessages_.end()) {
    vector<FixMessage> messages = move(it->second);
    pendingMessages_.erase(it);

    stringstream ss;
    ss << "Target [" << targetCompID << "] is online. Retrieved "
       << messages.size() << " messages.";
    LogInfo(ss.str());

    return messages;
  }

  return {};
}

// Print log file contents
void Logger::PrintLogFileContents() {
  lock_guard<mutex> lock(logMutex_);

  cout << "\n--- [START] Displaying Log File: " << logFilename_ << " ---" << endl;
  ifstream logFileIn(logFilename_);
  if (!logFileIn.is_open()) {
    cerr << "ERROR: Could not open log file: " << logFilename_ << endl;
    cout << "--- [END] Log File Display ---" << endl;
    return;
  }

  string line;
  while (getline(logFileIn, line)) cout << line << endl;
  logFileIn.close();

  cout << "--- [END] Log File Display ---" << endl;
}
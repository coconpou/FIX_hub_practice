#include "Logger.h"
#include <fstream>

Logger::Logger(const std::string& logFilename) {
    _logFile.open(_logFilename, std::ios::out | std::ios::app);
    if (!_logFile.is_open()) {
        std::cerr << "CRITICAL ERROR: Could not open log file for writing: " 
                  << _logFilename << std::endl;
    }

    LogInfo("Logger system initialized.");
}

Logger::~Logger() {
    LogInfo("Logger system shutting down.");
    
    if (_logFile.is_open()) { _logFile.close(); }
}

std::string Logger::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    // stand form YYYY-MM-DD HH:MM:SS
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

    // add ms
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void Logger::WriteLog(const std::string& level, const std::string& logMessage) {
    // lock log to prevent concurrent write
    std::lock_guard<std::mutex> lock(_logMutex);

    // cout log and write to file
    std::stringstream ss;
    ss << "[" << GetTimestamp() << "] "
       << "[" << std::setw(8) << std::left << level << "] "
       << logMessage;
    
    std::string logLine = ss.str();

    if (level == "ERROR") {
        std::cerr << logLine << std::endl;
    } 
    else {
        std::cout << logLine << std::endl;
    }

    if (_logFile.is_open()) {
        _logFile << logLine << std::endl; 
    }
    
}

void Logger::LogValidationResult(const std::string& sessionID, bool success, const std::string& reason) {
    std::stringstream ss;
    ss << "Session: [" << sessionID << "] "
       << (success ? "Validation SUCCESS" : "Validation FAILED")
       << ". Reason: " << reason;
    
    WriteLog("VALIDATE", ss.str());
}

void Logger::LogInfo(const std::string& message) {
    WriteLog("INFO", message);
}

void Logger::LogError(const std::string& message) {
    WriteLog("ERROR", message);
}


// --- offline msg ---

void Logger::QueueMessageForOfflineTarget(const std::string& targetCompID, const FixMessage& msg) {
    // lock
    std::lock_guard<std::mutex> lock(_queueMutex);

    _pendingMessages[targetCompID].push_back(msg);

    std::stringstream ss;
    ss << "Target [" << targetCompID << "] is offline. Queuing message.";
    LogInfo(ss.str());
}

bool Logger::HasPendingMessages(const std::string& targetCompID) {
    // lock
    std::lock_guard<std::mutex> lock(_queueMutex);

    // search map
    auto it = _pendingMessages.find(targetCompID);

    if (it != _pendingMessages.end() && !it->second.empty()) {
        return true;
    }
    
    return false;
}

std::vector<FixMessage> Logger::RetrievePendingMessages(const std::string& targetCompID) {
    // lock
    std::lock_guard<std::mutex> lock(_queueMutex);

    // search map
    auto it = _pendingMessages.find(targetCompID);

    if (it != _pendingMessages.end()) {
        std::vector<FixMessage> messages = std::move(it->second);
        _pendingMessages.erase(it);


        std::stringstream ss;
        ss << "Target [" << targetCompID << "] is online. Retrieving " 
           << messages.size() << " pending messages.";
        LogInfo(ss.str());

        return messages;
    }

    return std::vector<FixMessage>();
}

void Logger::PrintLogFileContents()
{

    std::lock_guard<std::mutex> lock(_logMutex);
    
    std::cout << "\n--- [START] Displaying Log File: " << _logFilename << " ---" << std::endl;

    std::ifstream logFileIn(_logFilename);

    if (!logFileIn.is_open()) {
        std::cerr << "ERROR: Could not open log file for reading: " << _logFilename << std::endl;
        std::cout << "--- [END] Log File Display ---" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(logFileIn, line)) {
        std::cout << line << std::endl;
    }

    logFileIn.close(); 
    std::cout << "--- [END] Log File Display ---" << std::endl;
}
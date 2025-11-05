#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <vector>
#include <map>
#include <mutex>      
#include <iostream>   
#include <sstream>    
#include <chrono>     
#include <iomanip>    
#include <fstream>

using FixMessage = std::map<int, std::string>;

class Logger {
private:

    std::string _logFilename;
    std::ofstream _logFile;

    // get時間戳記 "YYYY-MM-DD HH:MM:SS"

    std::string GetTimestamp() const;

    void WriteLog(const std::string& level, const std::string& logMessage);

    // 防止多線程同時寫入
    std::mutex _logMutex;

    std::map<std::string, std::vector<FixMessage>> _pendingMessages;

    std::mutex _queueMutex;
public:
    Logger(const std::string& logFilename = "fix_hub.log");

    ~Logger();

    // --- 日誌功能 ---
    void LogValidationResult(const std::string& sessionID, bool success, const std::string& reason);

    void LogInfo(const std::string& message);

    void LogError(const std::string& message);

    // --- 離線訊息 ---
    void QueueMessageForOfflineTarget(const std::string& targetCompID, const FixMessage& msg);

    bool HasPendingMessages(const std::string& targetCompID);

    std::vector<FixMessage> RetrievePendingMessages(const std::string& targetCompID);

    // --- 紀錄檔案 ---
    void PrintLogFileContents();

};// LOGGER_H_

#endif 
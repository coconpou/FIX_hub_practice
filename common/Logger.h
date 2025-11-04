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

using FixMessage = std::map<int, std::string>;

class Logger 
{
public:
    Logger();
    ~Logger();

    // --- 1. 記錄 FIX 驗證結果 (日誌功能) ---
    void LogValidationResult(const std::string& sessionID, bool success, const std::string& reason);

    void LogInfo(const std::string& message);

    void LogError(const std::string& message);

    // --- 2. 離線訊息佇列功能 ---
    void QueueMessageForOfflineTarget(const std::string& targetCompID, const FixMessage& msg);

    bool HasPendingMessages(const std::string& targetCompID);

    std::vector<FixMessage> RetrievePendingMessages(const std::string& targetCompID);


private:

    // 取得目前的時間戳記 "YYYY-MM-DD HH:MM:SS"

    std::string GetTimestamp() const;

    void WriteLog(const std::string& level, const std::string& logMessage);

    // 用於防止多線程同時寫入
    std::mutex _logMutex;

    std::map<std::string, std::vector<FixMessage>> _pendingMessages;

    std::mutex _queueMutex;
};

#endif // LOGGER_H_
#include "Logger.h"

Logger::Logger() 
{
    LogInfo("Logger system initialized.");
}

Logger::~Logger() 
{
    LogInfo("Logger system shutting down.");
}

// --- 1. 日誌功能 ---

std::string Logger::GetTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    // ms
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    // 格式 YYYY-MM-DD HH:MM:SS
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");

    // 附加毫秒
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void Logger::WriteLog(const std::string& level, const std::string& logMessage)
{
    // 使用 std::lock_guard 
    // 在離開函式後 _logMutex 自動解鎖
    // 防止多個 Session 同時寫log造成錯亂
    std::lock_guard<std::mutex> lock(_logMutex);

    // 看輸出
    std::cout << "[" << GetTimestamp() << "] "
              << "[" << std::setw(8) << std::left << level << "] "
              << logMessage << std::endl;
    
}

void Logger::LogValidationResult(const std::string& sessionID, bool success, const std::string& reason)
{
    std::stringstream ss;
    ss << "Session: [" << sessionID << "] "
       << (success ? "Validation SUCCESS" : "Validation FAILED")
       << ". Reason: " << reason;
    
    WriteLog("VALIDATE", ss.str());
}

void Logger::LogInfo(const std::string& message)
{
    WriteLog("INFO", message);
}

void Logger::LogError(const std::string& message)
{
    std::lock_guard<std::mutex> lock(_logMutex);
    std::cerr << "[" << GetTimestamp() << "] "
              << "[ERROR   ] "
              << message << std::endl;
}


// --- 2. 離線訊息 ---

void Logger::QueueMessageForOfflineTarget(const std::string& targetCompID, const FixMessage& msg)
{
    // 鎖定佇列
    std::lock_guard<std::mutex> lock(_queueMutex);

    _pendingMessages[targetCompID].push_back(msg);

    std::stringstream ss;
    ss << "Target [" << targetCompID << "] is offline. Queuing message.";
    LogInfo(ss.str());
}

bool Logger::HasPendingMessages(const std::string& targetCompID)
{
    // 鎖定佇列
    std::lock_guard<std::mutex> lock(_queueMutex);

    // 搜尋 map
    auto it = _pendingMessages.find(targetCompID);

    if (it != _pendingMessages.end() && !it->second.empty()) {
        return true;
    }
    
    return false;
}

std::vector<FixMessage> Logger::RetrievePendingMessages(const std::string& targetCompID)
{
    // 鎖定佇列
    std::lock_guard<std::mutex> lock(_queueMutex);

    // 搜尋 map
    auto it = _pendingMessages.find(targetCompID);

    if (it != _pendingMessages.end())
    {
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
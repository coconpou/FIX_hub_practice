#ifndef FIXCODER_H_
#define FIXCODER_H_

#include "FixHelper.h" 
#include <string>
#include <map>
#include <mutex>

class FixCoder 
{
private:
    std::map<std::string, std::string> _compIdToCustomId; // e.g. "ABC" -> "99900A"
    std::map<std::string, std::string> _customIdToCompId; // e.g. "99900A" -> "ABC"
    
    mutable std::mutex _mutex; 
public:
    FixCoder();
    ~FixCoder();

    // company id && custom id
    void AddRoute(const std::string& compId, const std::string& customId);

    // according to Tag 128 to get final CompID (e.g. "BROKER")
    // if error occurs, return empty string and set error message
    std::string GetTargetCompID(const FixMessage& msg, std::string& error) const;

    // transform message to fix msg
    bool TransformMessage(FixMessage& msg, std::string& error);

};

#endif // FIXCODER_H_
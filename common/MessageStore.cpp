#include "MessageStore.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Logger.h"

using namespace std;
namespace fs = std::filesystem;

// Get singleton instance
MessageStore &MessageStore::instance() {
  static MessageStore instance;
  return instance;
}

// Initialize message store directory
void MessageStore::init(const string &storagePath) {
  storagePath_ = storagePath;
  try {
    if (!fs::exists(storagePath_)) {
      if (fs::create_directories(storagePath_)) {
        // Logger::instance().LogInfo("Created message store directory: " + storagePath_);
      }
    }
    initialized_ = true;
  } catch (const fs::filesystem_error &e) {
    // Logger::instance().LogError("Failed to initialize message store: " + string(e.what()));
    cerr << "Failed to initialize message store: " << e.what() << endl;
    initialized_ = false;
  }
}

// Store raw FIX message string for offline target
bool MessageStore::storeMessage(const string &targetCompId, const string &rawMessage) {
  if (!initialized_) return false;

  fs::path targetDir = fs::path(storagePath_) / targetCompId;

  try {
    if (!fs::exists(targetDir)) fs::create_directories(targetDir);

    // Generate unique filename (timestamp)
    auto now = chrono::system_clock::now();
    auto timestamp = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();

    string filename = to_string(timestamp) + ".fix";
    fs::path filePath = targetDir / filename;

    ofstream file(filePath);
    if (file.is_open()) {
      file << rawMessage;
      file.close();
      return true;
    }
  } catch (const fs::filesystem_error &e) {
    // Logger::instance().LogError("Failed to store message for " + targetCompId + ": " + string(e.what()));
    cerr << "Failed to store message for " << targetCompId << ": " << e.what() << endl;
  }

  return false;
}

// Retrieve and clear queued messages for target
vector<string> MessageStore::getAndClearQueuedMessages(const string &targetCompId) {
  vector<string> messages;
  if (!initialized_) return messages;

  fs::path targetDir = fs::path(storagePath_) / targetCompId;
  if (!fs::exists(targetDir) || !fs::is_directory(targetDir)) return messages;

  try {
    vector<fs::path> files;
    for (const auto &entry : fs::directory_iterator(targetDir)) {
      if (entry.is_regular_file() && entry.path().extension() == ".fix") {
        files.push_back(entry.path());
      }
    }

    // Sort by filename (timestamp order)
    sort(files.begin(), files.end());

    for (const auto &filePath : files) {
      ifstream file(filePath);
      if (file.is_open()) {
        string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        messages.push_back(content);
        file.close();
        fs::remove(filePath);   // Delete after reading
      }
    }
  } catch (const fs::filesystem_error &e) {
    // Logger::instance().LogError("Failed to get/clear messages for " + targetCompId + ": " + string(e.what()));
    cerr << "Failed to get/clear messages for " << targetCompId << ": " << e.what() << endl;
  }

  return messages;
}
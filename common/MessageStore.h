#ifndef MESSAGE_STORE_H
#define MESSAGE_STORE_H

#include <string>
#include <vector>

// Singleton message store for offline FIX messages
class MessageStore {
 public:
  // Get singleton instance
  static MessageStore &instance();

  // Delete copy constructor and assignment
  MessageStore(const MessageStore &) = delete;
  MessageStore &operator=(const MessageStore &) = delete;

  // Initialize message store with root path
  void init(const std::string &storagePath);

  // Store a raw FIX message string for offline target
  bool storeMessage(const std::string &targetCompId, const std::string &rawMessage);

  // Retrieve and clear queued messages for target
  std::vector<std::string> getAndClearQueuedMessages(const std::string &targetCompId);

 private:
  // Private constructor
  MessageStore() = default;

  std::string storagePath_;    // Root directory for offline messages
  bool initialized_ = false;   // Initialization state
};

#endif   // MESSAGE_STORE_H
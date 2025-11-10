#ifndef FIXSERVER_H
#define FIXSERVER_H

#include <QDateTime>
#include <QHash>
#include <QList>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "common/ConfigManager.h"
#include "common/FixHelper.h"
#include "common/Logger.h"
#include "common/MessageStore.h"

// TCP server for FIX connections
class FixServer : public QTcpServer {
  Q_OBJECT

 public:
  // Constructor
  explicit FixServer(QObject *parent = nullptr);

  // Start listening on specified port
  bool start(quint16 port);

  // Setup signal handling
  static void setupSignalHandlers();

  // --- Static Helpers for Testing ---
  // Parse raw string into a FixMessage map
  static FixMessage parseFixMessage(const std::string &rawMsg);

  // Serialize a FixMessage map back into a raw string, calculating BodyLength and CheckSum
  static std::string serializeFixMessage(FixMessage &msg);

public slots:
  // Slot to handle the status dump signal
  void handleStatusSignal();

private slots:
  // Handle new client connection
  void onNewConnection();

  // Handle client disconnection
  void onClientDisconnected();

  // Handle incoming data from client
  void onReadyRead();

 private:
  QList<QTcpSocket *> clients_;               // Connected client sockets
  QHash<QTcpSocket *, QByteArray> buffers_;   // Data buffers per client
  QHash<std::string, QTcpSocket *> compIdToSocketMap_; // CompID -> Socket
  QHash<QTcpSocket *, std::string> socketToCompIdMap_; // Socket -> CompID
  QHash<QTcpSocket *, QDateTime> connectionTimeMap_; // Socket -> Connection Time

  // Parse raw string into a FixMessage map
  static FixMessage parseFixMessage(const std::string &rawMsg);

  // Serialize a FixMessage map back into a raw string, calculating BodyLength and CheckSum
  static std::string serializeFixMessage(FixMessage &msg);

  // Signal handling
  static void statusSignalHandler(int signal);
  static int sig_pipe[2];
};

#endif   // FIXSERVER_H
# C++ FIX Hub Practice Project

## 專案簡介

這是一個使用 C++17 和 Qt6 Networking 函式庫從頭開始建立的 FIX (Financial Information eXchange) Hub 模擬專案。專案包含一個 FIX 伺服器 (Hub) 和一個對應的客戶端，旨在模擬真實世界中 FIX 引擎的核心功能。

本專案**不依賴**任何現成的 FIX 引擎函式庫 (如 QuickFIX)，所有 FIX 訊息的解析、打包和 Session 層邏輯均為自行實現，以便於學習和理解底層細節。

整個環境已被容器化，可透過 Docker 和 Docker Compose 進行一鍵部署和測試。

## 核心功能

- **身分驗證 (Authentication)**: 伺服器根據 JSON 設定檔中的白名單 (`allowed_comp_ids`) 對連線的客戶端進行 CompID 驗證。
- **訊息驗證 (Validation)**: 伺服器會檢查收到的應用層訊息類型 (`MsgType`) 是否在支援列表 (`supported_msg_types`) 中，對不支援的訊息會回傳 **Session-Level Reject (35=3)**。
- **動態訊息路由 (Routing)**: 伺服器能根據訊息來源 (`source_comp_id`) 和指定的投遞目標 (`DeliverToCompID`, Tag 128) 進行訊息轉送。
- **自訂標籤注入 (Tag Injection)**: 在訊息轉送前，可根據路由規則 (`routing_rules`) 為訊息動態加入自訂的 FIX Tags。
- **離線訊息佇列 (Offline Queuing)**: 當訊息的目標方離線時，伺服器會自動將訊息儲存到檔案系統中。
- **上線自動重送 (Message Replay)**: 當離線的客戶端重新上線並登入後，伺服器會立即將所有屬於它的離線訊息按順序補發過去。
- **連線狀態監控 (Status Monitoring)**: 可透過傳送 `SIGUSR1` 訊號給伺服器進程，使其將當前所有連線的 Session 狀態傾印到日誌檔案中。
- **單元測試 (Unit Testing)**: 使用 **Catch2** 框架為核心模組（設定管理、訊息編碼、離線儲存）編寫了單元測試。
- **容器化環境 (Containerization)**: 專案提供完整的 `Dockerfile` 和 `docker-compose.yml`，可一鍵啟動包含一個伺服器和兩個客戶端的完整測試環境。

## 專案結構

```text
.
├── common/             # Server 和 Client 共用的程式碼 (ConfigManager, Logger, etc.)
├── server/             # FIX Server 專案
│   ├── src/
│   ├── config/fix_server.cfg
│   └── Makefile
├── client/             # FIX Client 專案
│   ├── src/
│   ├── config/fix_client.cfg
│   └── Makefile
├── tests/              # 單元測試 (Catch2)
│   ├── catch.hpp
│   ├── main_test.cpp
│   └── test_*.cpp
├── base.Dockerfile     # 用於編譯的基礎 Docker 映像檔
├── client_a.cfg        # Docker Compose 測試用的客戶端 A 設定
├── client_b.cfg        # Docker Compose 測試用的客戶端 B 設定
└── docker-compose.yml  # Docker Compose 服務編排
```

## 如何使用

有兩種方式可以建置和執行此專案：

### 方式一：Docker 環境 (推薦)

這是最簡單、最快的方式，能一鍵啟動包含一個伺服器和兩個客戶端的完整測試場景。

**環境需求**:
- Docker
- Docker Compose

**執行步驟**:
在專案根目錄下，執行以下指令：
```bash
docker-compose up --build
```
- `--build` 參數會強制重新建置所有映像檔。
- 啟動後，您會在終端機上看到 `server`, `client-a`, `client-b` 的日誌輸出。
- `client-a` 會在登入後，自動傳送一筆訂單訊息給 `client-b`，您可以觀察日誌來驗證路由功能。
- 若要測試離線訊息，可先註解掉 `docker-compose.yml` 中的 `client-b` 服務，啟動後再將其加回並重啟。

若要停止所有服務，按 `Ctrl+C`，然後執行 `docker-compose down`。

### 方式二：原生環境建置

如果您想在本地端手動編譯和執行。

**環境需求**:
- C++17 編譯器 (g++)
- `make`
- `pkg-config`
- **Qt6 開發函式庫** (在 Ubuntu 上可透過 `sudo apt-get install qt6-base-dev` 安裝)

**編譯**:
```bash
# 編譯伺服器
cd server/
make

# 編譯客戶端
cd ../client/
make

# 編譯並執行單元測試
cd ../tests/
make test
```

**執行**:
```bash
# 啟動伺服器
./server/build/server

# 在另一個終端機啟動客戶端
# (客戶端行為由 client/config/fix_client.cfg 決定)
./client/build/client

# 執行狀態查詢
# 1. 找到伺服器進程 PID
pgrep server
# 2. 傳送訊號
kill -SIGUSR1 <PID>
# 3. 查看日誌
tail -f server/server.log
```
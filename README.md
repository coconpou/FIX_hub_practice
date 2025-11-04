# FIX System (Server & Client)

## 📝 To-Do

- [ ] 實作 DeliverToCompID (Tag 128) 轉送邏輯
- [ ] 新增 Router 模組 (FixRouter / Queue)
- [ ] 加入 QuickFIX Session 驗證與錯誤回報
- [ ] 整合 Catch2 單元測試與 CI/CD Pipeline
- [ ] 完善 Docker Compose 環境（支援自動重啟與 volume 持久化）

---

## 📘 專案簡介

`fix_system` 是一個以 **C++17** 開發、基於 **QuickFIX** 的簡易 FIX 通訊範例，  
包含獨立的 **Server (Acceptor)** 與 **Client (Initiator)** 模組，  
並透過 Docker 容器化實現可重現的測試與部署環境。

---

## 📂 專案結構

fix_system/
├─ common/ # 共用模組 (FixHelper, Logger)
├─ server/ # Server 專案
│ ├─ src/
│ ├─ config/fix_server.cfg
│ ├─ Makefile
│ └─ Dockerfile
├─ client/ # Client 專案
│ ├─ src/
│ ├─ config/fix_client.cfg
│ ├─ Makefile
│ └─ Dockerfile
├─ tests/ # 單元測試 (Catch2)
│ ├─ test_FixEncode.cpp
│ ├─ test_Config.cpp
│ └─ test_Session.cpp
├─ spec/ # FIX 訊息格式定義 (e.g., FIX44.xml)
├─ base.Dockerfile # 共用基底映像檔
├─ Makefile # 頂層建置控制
└─ docker-compose.yml # 同時啟動 server / client 測試用

---

## ⚙️ 開發環境需求

- **OS:** Ubuntu 22.04 或相容版本
- **Compiler:** g++ (C++17 以上)
- **Library:** QuickFIX, pthread
- **Build System:** Make
- **Container:** Docker + Docker Compose

---

## 🛠️ 建置與執行

### 1️⃣ 建立基底映像檔

```bash
docker build -f base.Dockerfile -t fix_base .

2️⃣ 建立並啟動 Server / Client

docker build -f server/Dockerfile -t fix_server .
docker build -f client/Dockerfile -t fix_client .

3️⃣ 使用 docker-compose 啟動

docker-compose up

Server 會自動監聽 9876 Port，
Client 啟動後會自動進行 Logon 並送出一筆測試委託單 (35=D)。

⸻

🧩 測試

進入 tests/ 資料夾後可執行：

make run

或在容器中運行：

docker run --rm fix_base make -C tests run


⸻

📄 License

This project is distributed under the MIT License.
QuickFIX is licensed under the QuickFIX open-source license.
```

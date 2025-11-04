#  FIX Server Image
FROM base AS server

WORKDIR /app/server

# 複製原始碼與設定
COPY ../common /app/common
COPY . /app/server

# 編譯 server
RUN make clean && make server

# 開放 port
EXPOSE 9876

# 啟動命令
CMD ["./fix_server"]
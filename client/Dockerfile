#  FIX Client Image
FROM base AS client

WORKDIR /app/client

# 複製原始碼與設定
COPY ../common /app/common
COPY . /app/client

# 編譯 client
RUN make clean && make client

# 預設啟動 client 程式
CMD ["./fix_client"]
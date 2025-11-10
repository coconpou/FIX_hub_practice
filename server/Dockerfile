# Stage 1: Build the application
FROM fix-hub-base:latest AS builder

# Copy all source code
COPY . /app

# Build the server
WORKDIR /app/server
RUN make

# Stage 2: Create the final, lightweight image
FROM ubuntu:22.04

# Install only runtime dependencies
RUN apt-get update && \
    apt-get install -y libqt6core6 libqt6network6 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the compiled binary from the builder stage
COPY --from=builder /app/server/build/server .

# Copy the config file
COPY server/config/fix_server.cfg ./config/

# Set environment variable for shared libraries
ENV LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"

# Expose port and run
EXPOSE 5555
CMD ["./server"]


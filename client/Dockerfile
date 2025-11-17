# Stage 1: Build the application
FROM fix-hub-base:latest AS builder

# Copy all source code
COPY . /app

# Build the client
WORKDIR /app/client
RUN make

# Stage 2: Create the final, lightweight image
FROM ubuntu:22.04

# Install only runtime dependencies
RUN apt-get update && \
    apt-get install -y libqt6core6 libqt6network6 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the compiled binary from the builder stage
COPY --from=builder /app/client/build/client .

# Copy the default config file (will be overridden by docker-compose volumes)
COPY client/config/fix_client.cfg ./config/

# Copy the spec file
COPY spec ./spec

# Set environment variable for shared libraries
ENV LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"

# Default command (can be overridden in docker-compose)
CMD ["./client"]

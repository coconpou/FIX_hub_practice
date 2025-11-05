# Dockerfile for FixClient
FROM base
WORKDIR /app/client

# Copy project files
COPY . /app/client

# Build the client project
RUN make clean && make

# Run client after a short delay (wait for server)
CMD sleep 2 && ./build/fix_client
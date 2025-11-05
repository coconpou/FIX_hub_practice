# Dockerfile for FixServer

FROM base
WORKDIR /app/server

# Copy project files
COPY . /app/server

# Build the server project
RUN make clean && make

# Expose server port
EXPOSE 5555

# Run server
CMD ["./build/fix_server"]
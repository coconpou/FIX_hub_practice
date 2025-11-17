# Base Image: C++ & Qt6 Build Environment
FROM ubuntu:22.04

LABEL maintainer="fix_system"
ENV DEBIAN_FRONTEND=noninteractive

# Install basic system tools and dependencies for building
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    pkg-config \
    qt6-base-dev \
    libquickfix-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app


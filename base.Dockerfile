# Base Image: C++ QuickFIX & Qt6 Runtime
FROM ubuntu:22.04 AS base

LABEL maintainer="fix_system"
ENV DEBIAN_FRONTEND=noninteractive

# Install basic system tools and dependencies
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    wget \
    libssl-dev \
    libxml2-dev \
    libtool \
    autoconf \
    automake \
    && rm -rf /var/lib/apt/lists/*

# Install QuickFIX from source
RUN git clone --depth 1 https://github.com/quickfix/quickfix.git /opt/quickfix && \
    cd /opt/quickfix && \
    ./bootstrap && \
    ./configure && \
    make && \
    make install

# Set shared environment variables
ENV LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
WORKDIR /app
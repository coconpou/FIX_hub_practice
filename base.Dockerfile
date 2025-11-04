#  Base Image: C++ QuickFIX Runtime
FROM ubuntu:22.04 AS base

LABEL maintainer="fix_system_dev"
ENV DEBIAN_FRONTEND=noninteractive

# basic system and tools 
# 基礎系統與工具
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    wget \
    libssl-dev \
    libxml2-dev \
    && rm -rf /var/lib/apt/lists/*

# install QuickFIX
RUN git clone --depth 1 https://github.com/quickfix/quickfix.git /opt/quickfix && \
    cd /opt/quickfix && ./bootstrap && ./configure && make && make install

# setting sharing environment
ENV LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
WORKDIR /app
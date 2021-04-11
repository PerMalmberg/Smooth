
#ARG  ESP_IDF_VERSION=latest
FROM ubuntu:21.04

ARG DEBIAN_FRONTEND=noninteractive

# Install prerequisites
RUN apt-get update \
    && apt-get install --no-install-recommends -y \
		cmake \
		ninja-build \
		libsodium-dev \
		libmbedtls-dev \
		gcc-8 \
		g++-8 \
	&& rm -rf /var/lib/apt/lists/*

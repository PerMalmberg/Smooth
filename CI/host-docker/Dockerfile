ARG ESP_IDF_VERSION
FROM espressif/idf:${ESP_IDF_VERSION}

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

EXPOSE 8080
EXPOSE 8443

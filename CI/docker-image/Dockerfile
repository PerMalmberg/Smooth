
ARG  ESP_IDF_VERSION=latest
FROM espressif/idf:${ESP_IDF_VERSION}

# Install prerequisites
RUN apt-get update \
    && apt-get install --no-install-recommends -y \
		libsodium-dev \
		libmbedtls-dev \
		gcc-8 \
		g++-8 \
	&& rm -rf /var/lib/apt/lists/*

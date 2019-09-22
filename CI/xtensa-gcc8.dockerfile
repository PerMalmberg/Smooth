FROM ubuntu:19.04

ARG IDF_BRANCH=master
ENV IDF_BRANCH=${IDF_BRANCH}

ENV IDF_TOOLS_PATH=/esp/tools

# Install prerequisites
RUN apt update && \
    apt install --no-install-recommends -y \ 
	git wget libncurses-dev \
	flex bison gperf python python-pip \
	python-setuptools python-serial \
	python-click python-cryptography \
	python-future python-pyparsing \
	python-pyelftools cmake ninja-build \
	ccache \ 
	libusb-1.0 \
	libsodium-dev \
	libmbedtls-dev \
	gcc-8 \
	g++-8 \
	&& rm -rf /var/lib/apt/lists/*
	# Get ESP-IDF
RUN	cd / \
	&& mkdir esp \
	&& cd esp \
	&& git clone --recursive https://github.com/espressif/esp-idf.git
	# Checkout desired branch
RUN	cd /esp/esp-idf \
	&& git checkout ${IDF_BRANCH} \
	# Install IDF tools
	&& cd /esp/esp-idf \ 
	&& ./install.sh \
	# Remove dist files
	&& rm -rf /esp/tools/dist/

# Entrypoint
#COPY ./entrypoint.sh /
#COPY ./gnu-toolchain.cmake /
#RUN chmod +x /entrypoint.sh
#ENTRYPOINT [ "/entrypoint.sh" ]
#CMD []
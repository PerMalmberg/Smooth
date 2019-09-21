FROM ubuntu:19.04

ARG IDF_BRANCH=master
ENV IDF_BRANCH=${IDF_BRANCH}

# Install prerequisites
RUN apt update && \
    apt install --no-install-recommends -y git wget libncurses-dev \
	flex bison gperf python python-pip \
	python-setuptools python-serial \
	python-click python-cryptography \
	python-future python-pyparsing \
	python-pyelftools cmake ninja-build \
	ccache libusb-1.0 \
	&& rm -rf /var/lib/apt/lists/* \
	# Get ESP-IDF
	&& cd / \
	&& mkdir esp \
	&& cd esp \
	&& git clone --recursive https://github.com/espressif/esp-idf.git \
	# Checkout desired branch
	&& cd /esp/esp-idf \
	&& git checkout ${idf_branch} \
	# Install IDF tools
	&& cd /esp/esp-idf \ 
	&& ./install.sh \
	# Remove dist files
	&& rm -rf /root/.espressif/dist/

# Entrypoint
COPY ./entrypoint.sh /
RUN chmod +x /entrypoint.sh
ENTRYPOINT [ "/entrypoint.sh" ]
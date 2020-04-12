FROM ubuntu:19.10

ARG IDF_BRANCH=master
ENV IDF_BRANCH=${IDF_BRANCH}

ENV IDF_PATH=/esp/esp-idf
ENV IDF_TOOLS_PATH=/esp/tools

ARG DEBIAN_FRONTEND=noninteractive

# Install prerequisites
RUN apt update \
	&& apt upgrade -y \
    && apt install --no-install-recommends -y \ 
		git \
		wget \
		libncurses-dev \
		xz-utils \
		flex bison gperf \
		ca-certificates \
		ninja-build \
		ccache \ 
		libusb-1.0 \
		libsodium-dev \
		libmbedtls-dev \
		gcc-8 \
		g++-8 \
		libssl-dev \
		make \
		libz-dev \
		libffi-dev \
	&& ln -s /usr/bin/gcc-8 /usr/bin/gcc \
	&& ln -s /usr/bin/g++-8 /usr/bin/g++ \
	&& wget https://github.com/Kitware/CMake/releases/download/v3.17.1/cmake-3.17.1-Linux-x86_64.sh \
	&& chmod u+x cmake-3.17.1-Linux-x86_64.sh \
	&& ./cmake-3.17.1-Linux-x86_64.sh --skip-license --prefix=/usr/local \
	&& rm cmake-3.17.1-Linux-x86_64.sh \
	&& cd /usr/local/share/ca-certificates \
	&& wget https://curl.haxx.se/ca/cacert-2020-01-01.pem \
	&& update-ca-certificates \
	&& cd -
RUN wget https://www.python.org/ftp/python/3.8.2/Python-3.8.2.tar.xz \
	&& xz -d Python-3.8.2.tar.xz \
	&& tar xvf Python-3.8.2.tar \
	&& cd Python-3.8.2 \
	&& ./configure --enable-optimizations \
	&& make -j8 \
	&& make install \
	&& cd .. \
	&& rm -rf Python-3.8.2 \
	&& rm -rf Python-3.8.2.tar \
	&& pip3 install --upgrade pip virtenv setuptools click serial cryptography future pyparsing pyelftools \
	&& ln -s /usr/local/bin/python3 /usr/local/bin/python
# Get ESP-IDF

RUN cd / \
	&& mkdir esp \
	&& cd esp \
	&& git clone --recursive https://github.com/espressif/esp-idf.git \
	# Checkout desired branch
	&& cd ${IDF_PATH} \
    && echo "Checking out ${IDF_BRANCH}" \
	&& git checkout ${IDF_BRANCH} \
	&& git submodule update --init --checkout --recursive \
	# Install IDF tools
	&& cd ${IDF_PATH} \ 
	&& ./install.sh \
	# Remove dist files
	&& rm -rf /esp/tools/dist/

ENV IDF_CCACHE_ENABLE=1

COPY entrypoint.sh /entrypoint.sh
RUN chmod u+x /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]
CMD [ "/bin/bash" ]
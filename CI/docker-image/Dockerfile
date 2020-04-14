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
	&& rm -rf /var/lib/apt/lists/* \
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
	&& ln -s /usr/local/bin/python3 /usr/local/bin/python

# ESP-IDF
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

# As we're not using a virtenv, install Python packages needed for IDF globaly
RUN pip3 install --upgrade -r ${IDF_PATH}/requirements.txt

# Setup paths for IDF (these are taken from the output of ${IDF_PATH}/export.sh)
ENV PATH=${PATH}:"${IDF_PATH}/components/esptool_py/esptool"
ENV PATH=${PATH}:"${IDF_PATH}/components/espcoredump"
ENV PATH=${PATH}:"${IDF_PATH}/components/partition_table"
ENV PATH=${PATH}:"${IDF_PATH}/components/app_update"
ENV PATH=${PATH}:"${IDF_TOOLS_PATH}/tools/xtensa-esp32-elf/esp-2020r1-8.2.0/xtensa-esp32-elf/bin"
ENV PATH=${PATH}:"${IDF_TOOLS_PATH}/tools/xtensa-esp32s2-elf/esp-2020r1-8.2.0/xtensa-esp32s2-elf/bin"
ENV PATH=${PATH}:"${IDF_TOOLS_PATH}/tools/esp32ulp-elf/2.28.51-esp-20191205/esp32ulp-elf-binutils/bin"
ENV PATH=${PATH}:"${IDF_TOOLS_PATH}/tools/esp32s2ulp-elf/2.28.51-esp-20191205/esp32s2ulp-elf-binutils/bin"
ENV PATH=${PATH}:"${IDF_TOOLS_PATH}/tools/openocd-esp32/v0.10.0-esp32-20200406/openocd-esp32/bin"
ENV PATH=${PATH}:"${IDF_TOOLS_PATH}/python_env/idf4.2_py3.8_env/bin"
ENV PATH=${PATH}:"${IDF_PATH}/tools"

ENV IDF_CCACHE_ENABLE=1

# Since VSCode's Remote-Container extension at this time can't do COPY-commands (runs in the wrong directory)
# we add the source-command to the .bashrc file instead.
# COPY entrypoint.sh /entrypoint.sh
# RUN chmod u+x /entrypoint.sh
# ENTRYPOINT [ "/entrypoint.sh" ]
RUN echo ". $IDF_PATH/export.sh" >> ~/.bashrc

# Set locale so that "idf-py menuconfig" can be run.
RUN echo 'export LC_ALL="C.UTF-8"'  >> ~/.bashrc && echo 'export LC_CTYPE="C.UTF-8"'  >> ~/.bashrc


CMD [ "/bin/bash" ]
FROM ubuntu:19.04

ARG IDF_BRANCH=master
ENV IDF_BRANCH=${IDF_BRANCH}

# Install prerequisites
RUN apt update
RUN apt install -y git wget libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache
RUN apt install -y libusb-1.0

# Get ESP-IDF
RUN mkdir esp && cd esp && git clone --recursive https://github.com/espressif/esp-idf.git

# Checkout desired branch
RUN cd /esp/esp-idf && git checkout ${idf_branch}

# Install IDF tools
RUN export IDF_TOOLS_PATH=/esp/tools && cd /esp/esp-idf && ./install.sh
RUN echo "export IDF_TOOLS_PATH=/esp/tools" >> ~/.bashrc
RUN echo ". /esp/esp-idf/export.sh" >> ~/.bashrc

FROM espressif/idf

# Install prerequisites
RUN apt-get update \
    && apt-get install --no-install-recommends -y \
#		libncurses-dev \
#		xz-utils \
#		flex bison gperf \
#		ninja-build \
#		ccache \ 
#		libusb-1.0-0 \
		libsodium-dev \
		libmbedtls-dev \
		gcc-8 \
		g++-8 \
#		gdb \
#		libssl-dev \
#		make \
#		libz-dev \
#		libffi-dev \
	&& rm -rf /var/lib/apt/lists/*

EXPOSE 80
EXPOSE 8080
EXPOSE 443
EXPOSE 8443

CMD [ "/bin/bash" ]
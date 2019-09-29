#!/bin/bash

set -e

if [ -d /src/build ]; then
	rm -rf /src/build
fi

mkdir -p /src/build

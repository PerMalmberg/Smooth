#!/bin/bash

export IDF_TOOLS_PATH=/esp/tools
. /esp/esp-idf/export.sh

pushd /esp/esp-idf
git describe --tags
popd

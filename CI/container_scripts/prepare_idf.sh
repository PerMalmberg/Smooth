#!/bin/bash

. $IDF_TOOLS_EXPORT_CMD

pushd $IDF_PATH
echo "Git describe tags -----------------------------------------------------"
git describe --tags
popd

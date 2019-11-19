#!/bin/bash

set -e

. ./CI/docker_info.sh
. ./CI/startup_check.sh

# Run checks, ignoring stdout - stderr displays failed files.
docker run --rm --mount type=bind,source="$(pwd)",target="/cfg" --mount type=bind,source="$(pwd)/lib",target="/src" $uncrustify_image /do_formatting.sh
docker run --rm --mount type=bind,source="$(pwd)",target="/cfg" --mount type=bind,source="$(pwd)/test",target="/src" $uncrustify_image /do_formatting.sh

#  Reclaiming ownership of modified files after running above commands: 'sudo chown -R $USER:$USER *'
#!/bin/bash

set -e

echo "Building Host -------------------------------------------------------"
docker-compose run smooth ./CI//build_smooth_host.sh
echo "Running host tests ---------------------------------------------------"
docker-compose run -w /src/build_host/test/linux_unit_tests smooth ./linux_unit_tests
echo "Building esp32 test projects -----------------------------------------"
docker-compose run smooth ./CI/build_smooth_esp32.sh

#!/bin/bash

export $(grep -v '^#' ../../.env | xargs)

docker build --build-arg ESP_IDF_VERSION -t ${SMOOTH_IMAGE_NAME}:${ESP_IDF_VERSION} .


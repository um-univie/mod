#!/bin/bash
set -e
root=$(git rev-parse --show-toplevel)
cd $root
version=$(cat VERSION | tr -d '\n')
imageName="jakobandersen/mod:$version"
#cd build && rm mod-*.tar.gz && make dist && cd ..
docker build -t $imageName -f docker/Ubuntu.Dockerfile .
docker tag $imageName jakobandersen/mod:latest
echo "The docker image is $imageName"
echo "The image has been tagged with jakobandersen/mod:latest"

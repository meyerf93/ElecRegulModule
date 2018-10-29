#! /bin/sh

#
# Docker build command
#

docker build -f Dockerfile.rpi3 -t neighborhub/neighborhub:ElecRegulModule .
docker push neighborhub/neighborhub:ElecRegulModule

#!/bin/bash

docker run -e FILE=here/out.txt -e PORT=40001 -e SIZE=15000 -v $(pwd):/here -it --rm --name my-app udp-client

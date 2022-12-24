#!/bin/bash

# Accept-RangesがHTMLの範囲外
HOST="localhost"
PORT=5050
START=200
END=100

echo "~~~~~nginx~~~~~"
RANGE="${START}-${END}"
URL="${HOST}"
curl -v -r $RANGE $URL

echo "~~~~~webserv~~~~~"
RANGE="${START}-${END}"
URL="${HOST}:${PORT}"
curl -v -r $RANGE $URL

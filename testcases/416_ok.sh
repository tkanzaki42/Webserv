#!/bin/bash

# Accept-RangesがHTMLの範囲内(206を返す)
HOST="localhost"
PORT=5050
START=12
END=50

echo "~~~~~nginx~~~~~"
RANGE="${START}-${END}"
URL="${HOST}"
curl -v -r $RANGE $URL

echo "~~~~~webserv~~~~~"
RANGE="${START}-${END}"
URL="${HOST}:${PORT}"
curl -v -r $RANGE $URL

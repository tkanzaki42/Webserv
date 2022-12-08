#!/bin/bash

HOST="localhost"
PORT=5050
START=10
END=100

# Accept-RangesがHTMLの範囲内
RANGE="${START}-${END}"
URL="${HOST}:${PORT}"
curl -r $RANGE $URL

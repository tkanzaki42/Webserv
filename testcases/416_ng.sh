#!/bin/bash

HOST="localhost"
PORT=5050
START=200
END=100

# Accept-RangesがHTMLの範囲外
RANGE="${START}-${END}"
URL="${HOST}:${PORT}"
curl -r $RANGE $URL

START2=5000
END2=8000

# Accept-RangesがHTMLの範囲外
RANGE2="${START2}-${END2}"
curl -r $RANGE2 $URL

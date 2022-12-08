#!/bin/bash

HOST="localhost"
PORT=5050
LENGTH=8175

# パスが8175文字のURL(OK)
URL="${HOST}:${PORT}/?"
for i in `seq ${LENGTH}`
do
URL+="A"
done
curl $URL

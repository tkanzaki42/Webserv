#!/bin/bash

HOST="localhost"
PORT=5050
LENGTH=8176

# パスが8176文字のURL(NG)
URL="${HOST}:${PORT}/?"
for i in `seq ${LENGTH}`
do
URL+="A"
done
curl $URL

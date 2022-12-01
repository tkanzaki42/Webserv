#!/bin/bash

# 10文字のHTTPリクエストヘッダ(Success)
HEADER="HOGE:"
for i in `seq 5000`
do
HEADER+="X"
done
curl -H $HEADER localhost:5050

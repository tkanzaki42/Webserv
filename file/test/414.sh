#!/bin/bash

# デフォルトでlocalhostにアクセス
if [ $# -lt 1 ]; then
	HOST="localhost"
else
	HOST=$1
fi

# デフォルトで5050文字のポートにアクセス
if [ $# -lt 2 ]; then
	PORT=5050
else
	PORT=$2
fi

# デフォルトで10000文字のHTTPリクエストヘッダを送る
if [ $# -lt 3 ]; then
	LENGTH=5000
else
	LENGTH=$3
fi

# 1293文字のURL(Success)
URL="${HOST}:${PORT}/?"
for i in `seq ${LENGTH}`
do
URL+="A"
done
curl $URL

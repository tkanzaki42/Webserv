#!/bin/bash

# 1293文字のURL(Success)
URL1="localhost:5050/?"
for i in `seq 1277`
do
URL1+="A"
done
curl $URL1

# 1294文字のURL(Crash)
# URL2="localhost:5050/?"
# for i in `seq 1278`
# do
# URL2+="A"
# done
# curl $URL2

# 7937文字のURL(Crash)
# URL3="localhost:5050/?"
# for i in `seq 7920`
# do
# URL3+="A"
# done
# curl $URL3

# 7937文字のURL(Not crash)
# URL4="localhost:5050/?"
# for i in `seq 7921`
# do
# URL4+="A"
# done
# curl $URL4


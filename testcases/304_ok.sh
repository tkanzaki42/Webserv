#!/bin/bash


# curl localhost:5050 -v で Etag と Last-Modified を確認
echo "~~~~~nginx~~~~~"
curl -v localhost
# curl -v localhost -H "If-None-Match: W/\"[Etagの値]\""" -H "If-Modified-Since: [Last-Modifiedの値]"

echo "~~~~~webserv~~~~~"
curl -v localhost:5050
# curl -v localhost:5050 -H "If-None-Match: W/\"[Etagの値]\""" -H "If-Modified-Since: [Last-Modifiedの値]"

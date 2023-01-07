#!/bin/bash


# curl localhost:6060 -v で Etag と Last-Modified を確認
echo "~~~~~nginx~~~~~"
curl -v localhost
# curl -v localhost -H "If-None-Match: W/\"[Etagの値]\""" -H "If-Modified-Since: [Last-Modifiedの値]"

echo "~~~~~webserv~~~~~"
curl -v localhost:6060
# curl -v localhost:6060 -H "If-None-Match: W/\"[Etagの値]\""" -H "If-Modified-Since: [Last-Modifiedの値]"

# ./webserv configs/valid/13_autoindex.conf
# brew services start nginx (autoindexの設定をonでサーバー起動)
echo "~~~~~nginx~~~~~"
curl -v localhost/
echo;
echo;
echo "~~~~~webserv~~~~~"
curl -v localhost:5050/

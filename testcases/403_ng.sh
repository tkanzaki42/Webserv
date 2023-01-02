# ./webserv configs/valid/01_host.conf
# brew services start nginx (autoindex / indexの設定をしないでサーバー起動)
echo "~~~~~nginx~~~~~"
curl -v localhost:8080/
echo;
echo;
echo "~~~~~webserv~~~~~"
curl -v localhost:5050/

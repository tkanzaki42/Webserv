# Webserv
[RFC9112 (HTTP/1.1)](https://httpwg.org/specs/rfc9112.html)、[RFC3875 (CGI/1.1)](https://datatracker.ietf.org/doc/html/rfc3875)に準拠した、シンプルなウェブサーバです。

※一部の仕様は実装していません。


## 機能

- GET、POST、DELETEの各メソッドに対応
- バーチャルホスト
- 複数ポートでの起動
- リダイレクト
- ファイルのアップロード
- location指定によるパスごとの設定切り替え
- 自作エラーページの指定
- Autoindex
- CGIの実行（バイナリは拡張子.out、スクリプトは.pyのみ）
- Basic認証
- ETagによるキャッシュ管理
- Accept-Rangesによるページング
- チャンクデータの受信


## 前提環境

MacまたはLinux

テスト環境
- Darwin Kernel Version 18.7.0
- Ubuntu 20.04.2 LTS


## インストール

### コンパイル

```
make
```

### デバッグ用コンパイル

```
make debug
```
fsanitizeオプションを付加してコンパイルします。

その他、all、clean、fclean、reオプションが使えます。詳細は`make usage`を参照。


## サーバの起動と停止

### サーバ起動

- default.confで起動する

```bash
./webserv
```

- 指定したコンフィグで起動する（&をつけるとバックグラウンドで起動する）

```bash
./webserv configs/review/01_multi_port.conf &
```

### サーバの起動状態を確認

```bash
% ps
PID TTY           TIME CMD
79846 ttys000    0:00.03 -zsh
79958 ttys000    0:00.00 ./webserv configs/review/01_multi_port.conf
```

### サーバ停止

```bash
pkill webserv
```


## アクセス方法

### ブラウザからのアクセス

http://localhost:6060/index.html をChromeなどで表示してください。

### curlからのアクセス

- 通常アクセス

```bash
curl localhost:6060/index.html
```

`-v`オプションをつけると、リクエストヘッダ、レスポンスヘッダを表示できる

- Hostヘッダをつけて接続

```bash
curl -H'Host: hoge.com' localhost:6060
```

- POSTによるデータ送信

```bash
curl -X POST -H"Content-Type:plain/text" --data "12345" localhost:6060/
```

- 名前解決の一時的な設定

```bash
curl --resolve example.com:6060:127.0.0.1 http://example.com:6060/index.html
```

### telnetからのアクセス

```bash
% telnet localhost 6060
GET / HTTP/1.1
Host: localhost:6060
```

## 設定項目

### 最小設定

```
server
 location /
  {root|./public_html}
 listen
  {6060}
```

- 1つのサーバがserver項目1つに対応
- 行頭のスペース1つがインデント1つに対応
- リクエストURLで指定されるパスをlocationに書き、レスポンスで返すファイルが置かれているパスをrootに書く
- ポート番号を指定する

### 複数サーバ設定例
```
server
 server_name
  example.com
 location /
  {root|./public_html,index|index.html}
 listen
  {6060}

server
 server_name
  hoge.com
 location /
  {root|./public_html/subdir,index|index.html}
 listen
  {6060}
```

- server_nameでバーチャルホストを指定
- 各サーバごとに設定を記載する
- バーチャルホストでなく、ポート番号でサーバを複数記述してもよい(その場合はserver_name項目はなくてもよい)

### CGI設定例

```
server
 location /cgi-bin
  {root|./public_html/cgi-bin,cgi_extension|out|py,upload_store|./upload}
 location /
  {root|./public_html,index|index.html}
 listen
  {6060}
```

- cgi_extensionで、実行を許可するCGIの拡張子を指定する
- POSTメソッドを使用する場合は、POSTで送付されるデータを保存するパスをupload_storeで指定する（upload_fileというファイル名で保存される）

### その他

その他いくつか設定可能な項目があります。configsディレクトリにサンプルがあるので参考にしてください。

## 負荷テスト

siegeを使用して、負荷テストを実行できます。

### siegeインストール

- brewインストール ※42環境限定

```
curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
```
brewインストール後、一度exitして再ログインしてください。

- siegeインストール

```
brew install siege
```

### siege実行

```
make siege
```
または、直接siegeコマンドを実行。
```
siege -c 100 -t 30S -b http://localhost:6060/empty.html
```

# Webserv
[RFC9112 (HTTP/1.1)](https://httpwg.org/specs/rfc9112.html)、[RFC3875 (CGI/1.1)](https://datatracker.ietf.org/doc/html/rfc3875)に準拠した、シンプルなウェブサーバです。

※一部の仕様は実装していません。

## 機能

- GET、POST、DELETEの各メソッドに対応

## インストール

### コンパイル

```
make
```

## 使用方法

```
./webserv
```
default.confを使用して起動します。


```
./webserv configs/valid/01_host.conf
```
第一引数にコンフィグファイルを指定できます。

## 設定項目

## 負荷テスト

siegeを使用して、負荷テストを実行できます。

## siegeインストール

## siege実行

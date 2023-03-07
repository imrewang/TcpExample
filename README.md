# TcpExample

Tcp连接的简单示例

环境：ubuntu

### 1.服务端tcp_server

生成可执行文件： gcc tcp_server.c -o server

运行：./server 虚拟机的ip地址 选择的端口

### 2.客户端tcp_client

再开一个终端

生成可执行文件： gcc tcp_client.c -o client

运行：./client 虚拟机的ip地址 选择的端口

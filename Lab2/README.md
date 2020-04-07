# Lab2: Your Own HTTP Server | HTTP服务器

*一些材料源于加州大学伯克利分校 CS162 课程之作业二，特此鸣谢。*

* 在本地 CloudComputingLabs 文件夹中使用 `git pull` 指令从实验仓库中获取最近的提交（commit）。

* 实验指导书为 `Lab2/README English.ver.md` ，本文档其翻译版本。
* 所有材料置于 `Lab2/` 文件夹内。



## 1. Overview 实验概述

使用本课习得网络编程知识，各小组**实现一个 HTTP 服务器**。同时使用高度并发编程技巧，以保证网络服务器（web server）的性能。

### Goals 目标

* 习得基础的网络编程能力，如使用 Socket API、解析数据包（Parsing Packets）。
* 熟悉强大且高性能的并发编程技巧。



## 2. Background 背景

### 2.1 Hypertext Transport Protocol 超文本传输协议

* 超文本传输协议（HTTP）是今时普遍使用的网络应用层的协议。
* 和许多网络协议一样，HTTP 采用了**客户端-服务器模型架构 / 主从式架构**。
  * 一 HTTP 客户端网络连接到服务器后，发送一个 HTTP 请求报文。 
  * 服务器返回以一条 HTTP 响应报文，通常包括客户端请求的资源（文本、文件、二进制数据等）。
* 此处简短介绍 HTTP 报文及其结构。详细定义请看 [RFC 2626 - Hypertext Transfer Protocol - HTTP/1.1](https://tools.ietf.org/html/rfc2616)。

### 2.2 HTTP Messages / HTTP 报文

* HTTP 报文是一堆简洁、具有固定格式的数据。
* 所有 HTTP 报文最终均可归类为两类：**请求**报文、**响应**报文。
  * 请求报文即向网络服务器发送动作请求 / 响应报文携请求之结果返回客户端
  * 所有请求&响应报文都有相同的报文基础结构。

#### 2.2.1 Message Format 报文格式

```
	 HTTP-Message = start-line
					*(header-field CRLF)
					CRLF
					[message-body]
```

* HTTP 请求&响应报文由三部分组成：
  * Start Line 起始行 | 对报文进行描述
  * Header 首部 | 包含属性的首部块
  * Entity Body 实体主体 | 可选的包含数据的主体部分
  
  ###### 2.2.1.1 Start Line 起始行
  
  * 所有 HTTP 报文始于起始行。
  
  * 请求报文之起始行称为「**Request Line 请求行**」，用以叙述**做什么**（*what to do*），即**请求报文请求服务器对资源进行一些操作**。内容如下：
  
    * Method 方法 | 告知服务器需要做什么操作，如GET、PUT、POST等
    * Request target (URL usually.) | 描述方法操作之对象（宾语）
    * HTTP version / HTTP 版本 |  告知客户端 HTTP 适配版本，作为对期望响应版本的指示符
    * [例] `GET /index.html HTTP/1.0`
  
  * 响应报文之起始行称为「**Response/Status Line 响应行/状态行**」，用以叙述**发生什么**（*what happened*），即**承载状态信息和操作产生的所有结果数据**。内容如下：
  
    * HTTP version / HTTP 版本（HTTP/1.1 usually.）
    * Status Code 状态码 | 表明请求的状态（成功/失败）
    * Status Text 状态文本 | 对状态码的简短文本描述
    * [例] `HTTP/1.0 200 OK`
  
    <img src="images/startline" alt="index page curl" title="index page curl" style="zoom:100%;" />
  
  ###### 2.2.1.2 Header 首部
  
  起始行之后即零/一/多个 HTTP 首部字段。其在报文中添加了附加信息，本质上只是「**名：值**」列表。
  
  * 首部分类 
  
    * General Headers 通用首部 | 出现于请求/响应报文中
    * Request Headers 请求首部 | 提供更多关于请求的信息
    * Response Headers 响应首部 | 提供更多关于响应的信息
    * Entity Headers 实体首部 | 描述主体长度和内容，或资源本身
    * Extension Headers 扩展s首部 | 规范未明确定义，可由应用程序自定义的新首部
  
  * 简单语法 | 名字+(冒号)+(可选空格)+字段值。最后是一个 CRLF。
  
  * 此处不展开介绍，且**不强制要求于本实验中实现**。[More information in RFC 2616 - HTTP/1.1](https://tools.ietf.org/html/rfc2616).
  
  * Example of headers in a request:
  
    ```
    Host: 127.0.0.1:8888
    User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:74.0) Gecko/20100101 Firefox/74.0
    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
    Accept-Language: en-US,en;q=0.5
    Accept-Encoding: gzip, deflate
    Connection: keep-alive
    Upgrade-Insecure-Requests: 1
    Cache-Control: max-age=0
    								     // CRLF
    ```
  
    Example of headers in a response:
  
    ```
    Server: Guo's Web Server
    Content-length: 248
    Content-type: text/html
    									// CRLF
    ```
  
  
  ###### 2.2.1.3 Entity Body 实体的主体
  
  HTTP 报文第三部分：可选的实体的主体，是 HTTP 报文的载荷，即 HTTP 被设计出来希望传输的内容。
  
  * HTTP 报文可负载多种类电子数据：图像、流媒体、HTML 文档、应用程序、信用卡事务、电子邮件等。
  
  * Example of entity body:
  
    ```
    <html><head>
    <title>CS06142</title>
    </head><body>
    <h1>CS06142</h1>
    <p>Welcome to Cloud Computing Course.<br />
    </p>
    <hr>
    <address>Http Server at ip-127-0-0-1 Port 8888</address>
    </body></html>
    ```
  

#### 2.2.2 Structure of HTTP Request / HTTP 请求报文之构成

HTTP 请求报文：一 HTTP 请求行（一方法、一请求对象、一 HTTP 版本）+ 零/一/多个首部 + 空行（如CRLF）。

Example of HTTP request message:

```
GET /index.html HTTP/1.0   
Host: 127.0.0.1:8888
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:74.0) Gecko/20100101 Firefox/74.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Cache-Control: max-age=0
								     // CRLF
```

#### 2.2.3 Structure of HTTP Response / HTTP 响应报文之构成

HTTP 响应报文：一 HTTP 响应/状态行（一 HTTP 版本、状态码、状态文本）+ 零/一/多个首部 + 空行（如CRLF）+ HTTP 请求报文所请求的内容。

<img src="images/HTTP_Response_Headers2.png" alt="Not implemented" title="Not implemented" style="zoom:100%;" />

或老师提供例子：

```
HTTP/1.0 200 OK  					
Server: Tiny Web Server
Content-length: 248
Content-type: text/html
									// CRLF
<html><head>
<title>CS06142</title>
</head><body>
<h1>CS06142</h1>
<p>Welcome to Cloud Computing Course.<br />
</p>
<hr>
<address>Http Server at ip-127-0-0-1 Port 8888</address>
</body></html>
```

### 2.3 HTTP Proxy / HTTP 代理

* HTTP 代理服务器在服务器和客户端之间扮演「中介者」角色，在两者间往返传送 HTTP 消息。
* HTTP 代理服务器将代表客户端，完成相关 HTTP 事务。
  * 没有 HTTP 代理，HTTP 客户端直接与 HTTP 服务器通信。
  * 使用 HTTP 代理，客户端与代理对话，代理本身又代表客户端与服务器通信。
* HTTP 代理服务器既是 Web 服务器（处理连接并响应）又是 Web 客户端（发送请求并接收响应）。

* HTTP 代理之运行模式如下图所示：

```
                               +-----------+               +-----------+
                               |           |               |           |
   +----------+    Request     |           |   Request     |           |
   |          |+--------------->           |+-------------->           |
   |  Client  |                |   Proxy   |               |   Server  |
   |          <---------------+|           <--------------+|           |          
   +----------+	   Response    |           |   Response    |           |          
                               |           |               |           |
                               +-----------+               +-----------+
```



## 3. Your Lab Task 实验任务 

### 3.1 实现一个 HTTP Server

* 本实验教师/助教不会提供任何基础代码（any basic code）。
* 自己从头开始，实现一个满足下列必要条件的 HTTP server：

#### 3.1.1 HTTP Server Outline 服务器概述 / 目标

从网络角度，本实验的 HTTP 服务器应实现以下功能：

1. 创建一个持续监听的套接字（listening Socket），将其绑定到端口。
2. 等待客户端连接端口
3. 接受客户端连接并获取新的连接套接字。
4. 读入并解析 HTTP 请求
5. 提供服务：
   1. 处理 HTTP 的 GET/POST 请求。如有错误，返回错误信息。
   2. 将请求报文代理到另一个 HTTP 服务器（可选 / 进阶版本）

服务器将具有代理模式与非代理模式。服务器不会同时处于两种模式。代理部分详见 2.3 节。

#### 3.1.2 Handle HTTP request 处理 HTTP 请求

本实验只要求 HTTP 服务器实现 GET、POST 两种方法。也即，如若 HTTP 服务器收到非该两种方法的 HTTP 请求，只需返回「501 Not Implemented（未实现）」错误信息（响应报文状态码写明 501，详见 2.2 节）。

###### 3.1.2.1 Handle HTTP GET request 处理 HTTP GET 请求

HTTP 服务器应处理 GET 请求，以获取到 HTML 页面。

* HTTP 请求的路径指向 HTML 页面文件，则返回「200 OK」与 HTML 页面文件。【例】请求 `GET /index.html` 且 index.html 文件确实存在，则返回以上内容。同时也可以处理请求子目录的文件。【例】`GET /images/hero.jpg`
* HTTP 请求的路径指向一个目录，并且该目录包含 index.html 文件，则返回「200 OK」与 HTML 页面文件所在目录的所有内容。
* HTTP 请求的路径指向不存在的页面文件，或指向的目录不包含 index.html 文件，返回「404 Not Found」。

###### 3.1.2.2 Handle HTTP POST request 处理 HTTP POST 请求

本实验要求 HTTP 服务器实现的 POST 请求之处理非常简单。

1.  构造一个 HTTP POST 请求（详见 3.1.7.2 节），其包含「Name」和「ID」两字段值（请分别输入名字与学号），并发送至服务器的 `/Post_show` 文件夹内。【例】服务器 `IP: 127.0.0.1; Port: 8888`，则文件夹路径 `http://127.0.0.1:8888/Post_show`。

若 HTTP 服务器收到符合上述情况的 POST 请求，则返回「200 OK」及已发送的「名字 - 学号」对。详见 3.1.7.2.

2. 不符合上述情况（请求路径不是 `/Post_show` 或字段非「Name」和「ID」），返回「404 Not Found」。

###### 3.1.2.3 Other request 其它请求

若遇 DELETE、PUT 等方法，只管返回「501 Not Implemented（未实现）」错误信息。详见 3.1.7.3 节。

#### 3.1.3 Implement a proxy server (optional) 实现代理服务器（可选）

使服务器将 HTTP 请求代理到另一个 HTTP 服务器，并将响应报文转发给客户端。

1. 使用命令行参数 `--proxy`，该值包含上游 HTTP 服务器的地址和端口号。
2. 代理服务器应在两个套接字（HTTP 客户端 fd 和上游 HTTP 服务器 fd）上等待新数据。当数据到达时，应立即将其读取到缓冲区，然后写入另一个套接字。本质上，这是在维护 HTTP 客户端和上游 HTTP 服务器之间的双向通信。请注意，代理服务器必须支持多个请求/响应。
3. 如果其中一个套接字关闭，则通信无法继续，因此您应该关闭另一个套接字并关闭子进程。

提示：1）该实现难于写入文件或从stdin读取，因为不清楚双向通信哪一侧先写入数据，或者在收到响应后是否会写入更多数据。 2）再次使用多线程执行此任务。例如，考虑使用两个线程来促进双向通信效率，一个线程从A到B，另一个线程从B到A。

#### 3.1.4 Use multi-thread to increase concurrency 使用多线程增加并发

HTTP 服务器应通过多线程并发处理客户端尽可能多的请求。可从（但不限于）以下三个选项选择以构建多线程服务器：

* **按需求布置线程（On-demand threads）。**
  * 当新客户端连接时，创建新线程，以之全力处理该客户端的任务（包括解析 HTTP 请求、取回页面文件、发送响应报文等）。
  * 任务完成，线程即焚。
  * 如通过 TCP `recv()` 探测对话之完成，然而在 HTTP 层面上探测客户端任务之完成较难。
* **常在线程的线程池？（A pool of always-on threads）。**
  * HTTP 服务器程序利用固定大小的线程池来并发处理多客户端请求。
  * 无任务时，线程于等待状态。
  * 新客户端连接，分配一个线程以处理并返回响应报文。
  * 受分配的线程若繁忙，则可以使用工作队列来缓冲请求，使线程稍后对其处理。
* **混合。**混合以上两种方法。例如，使用线程池来接收请求、发送响应，使用按需求线程取回大的页面文件。

随意使用上述三种方法，或者使用其它你觉得 COOL😎 的多线程架构。

#### 3.1.5 Specify arguments 明确参数

程序应能使用 long options 接受多参数输入，且在启动时就指定这些参数：

* `--ip` 			——指定服务器 IP 地址
* `--port`         ——指定 HTTP 服务器正在聆听客户端前来连接的端口
* `--proxy`       ——[可选] 指定上游 HTTP 服务器以便代理。参数后可有一个冒号+端口号。无端口号默认80。
  [例] `http://www.CS06142.com:80`

若对 long options 没有概念，可参阅[此](https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html#Argument-Syntax)。可能使用到的函数包括：`getopt_long()`, `getopt_long_only()`, `getopt()` 等。请使用 `man` 指令查看这些函数的用法。

#### 3.1.6 Run your HTTP Server 运行 HTTP 服务器

程序应能运行于终端（Terminal）。假设程序名为 *httpserver*：

* 非代理模式：`./httpserver --ip 127.0.0.1 --port 8888 --number-thread 8`  / 表示 HTTP 服务器 IP 地址为 127.0.0.1、服务端口为 8888、使用 8 线程。
* 代理模式（可选）：`./httpserver --ip 127.0.0.1 --port 8888 --number-thread 8 --proxy https://www.CS06142.com:80`  / 表示 HTTP 服务器 IP 地址为 127.0.0.1、服务端口为 8888、使用 8 线程、上游代理服务器为 `https://www.CS06142.COM:80`。

当输入以上命令并回车，HTTP 服务器应立刻启动。

#### 3.1.7 Accessing Your HTTP Server 测试 HTTP 服务器

###### 3.1.7.1 Using GET method 使用 GET 方法

* 打开浏览器并输入 URL，以检测 HTTP 服务器是否运行。**[⚠]127.0.0.1 是本地 IP 地址。**所以可以使用该 IP 地址在本地机器上测试 HTTP 服务器。

  【例】

<img src="images/index.png" alt="index page" title="index page" style="zoom:43%;" />

* 使用 `curl` 程序来发送 HTTP 请求。

  【例】`curl -i -X GET http://127.0.0.1:8888/index.html`

<img src="images/index_curl.png" alt="index page curl" title="index page curl" style="zoom:50%;" />

* 若请求页面不存在，HTTP 服务器应返回「404 Not Found」错误信息。

  <img src="images/get_404_curl.png" alt="not found page curl" title="not found curl" style="zoom:50%;" />

###### 3.1.7.2 Using POST method 使用 POST 方法

* 使用 `curl` 程序来发送使用 POST 方法的 HTTP 请求。

  【例】`curl -i -X POST --data 'Name=HNU&ID=CS06142' http://127.0.0.1:8888/Post_show`

  <img src="images/post_curl.png" alt="post curl" title="post curl" style="zoom:50%;" />

* 若请求 URL 不是 `/Post_show` 或者字段不是「Name & ID」，就返回「404 Not Found」错误信息。

  <img src="images/post_404_curl.png" alt="post not found curl" title="post not found curl" style="zoom:50%;" />

* 也可以构建 POST HTTP 请求，并使用浏览器插件工具发送该请求至 HTTP 服务器。

###### 3.1.7.3 Other method 其它方法

* HTTP 服务器不负责处理除 GET 请求、POST 请求以外的请求。

* 若发送了 DELETE、PUT、HEAD 等请求，应得到「501 Not Implemented」响应报文。

  <img src="images/not_implemented_curl.png" alt="Not implemented" title="Not implemented" style="zoom:50%;" />
  
  <img src="images/not_implemented.png" alt="Not implemented" title="Not implemented" style="zoom:50%;" />

#### !!!3.1.8 Implementatiom requirements 实现的必要条件!!!

###### 3.1.8.1 Basic Version 基础版本

* 程序应完成 **`3.1.1~3.1.7` 节中、除 `3.1.3` 节外的所有任务**。
* **每个正在运行的 TCP 连接仅处理一个请求（one request per TCP connection）**。
* 客户端等待响应，当得到响应时可能会复用该 TCP 连接（或建立新的 TCP 连接）以传送新的请求。

###### 3.1.8.2 Advanced Version 进阶版本

* 程序应完成 **`3.1.1~3.1.7` 节中、包括 `3.1.3` 节的所有任务**。
* **可以在一个 TCP 连接出发多个 HTTP 请求（multiple http requests per TCP connection）**，也即「**管道传输**」。许多浏览器与服务器（如 Chrome）均支持此法传输。
* 来自同一 TCP 连接的 HTTP 请求应以请求顺序响应。使用多线程时请注意顺序问题。

### 3.2 Finish A Performance Test Report 完成性能测试报告

* 进行测试，然后将测试报告和实验代码以同提交到 Github 仓库中。
* 测试报告应描述**在不同测试环境下的测试结果**。明确而言，内容如下述：
  1. **测试在不同服务器运行环境下，每秒处理多少 HTTP 请求。**例如，变更 CPU 核心数、启用/禁用超线程。
  2. **测试在不同（同时向服务器发送请求的）并发客户端数情况下，每秒处理多少 HTTP 请求。**更改客户端的工作负载（Do Change the clients‘ workload）。例如，测试客户端何时将新 TCP 连接用于新请求，或何时将旧 TCP 连接复用于新请求。（此外，进阶版本的服务器需要尝试更改同一客户端 TCP 连接上的出站请求数）

* 可以自己编写一个简单的发送 HTTP 请求的程序（可以在同一机器运行多个客户端程序，以假滥竽）。
* 或者使用现成 HTTP 客户端测试程序，如 [ab - Apache HTTP server benchmarking tool](http://httpd.apache.org/docs/current/programs/ab.html)。

[⚠] 注意客户端可能存在性能瓶颈。因此最好使用多台机器进行程序性能测试。例如，可以使用（三个组员的）三台机器，每台机器运行多客户端进程，并在另一（组员的）机器上运行服务器进程。

[⚠] 注意网络可能存在瓶颈。因此需根据网络环境的物理带宽来估计程序性能极限，并查看该 HTTP 服务器的实现能否达到性能极限。



## 4. Lab Submission 实验提交

* 将所有代码放入 `Lab2` 文件夹中。
* 编写 Makefile 以便助教以一简单命令 `make` 编译程序。
* **编译好的二进制可执行文件应名为「httpserver」并置于 `Lab2` 文件夹，即运行路径 `./Lab2/httpserver`**
* **性能测试报告名为「Lab2-test-report.pdf」（及md文档）并置于 `Lab2` 文件夹。**
* 谨遵上旨，以便助教进行自动测试。
* 谨从指导 `实验总述 翻译.md` 以正确提交实验。

**⚠  DDL：2020年4月29日上午10点整**



## 5. Grading Standards 得分标准

* **23 分 |** ①完成基础版本；②性能测试报告满足两项最低要求。
* **25 分 |** ①完成进阶版本；②性能测试报告满足两项最低要求。

 **⚠  满则盈，缺稍殆。历史至上。**
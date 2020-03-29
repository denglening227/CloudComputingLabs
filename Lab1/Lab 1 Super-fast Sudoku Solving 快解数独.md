# Lab 1: "Super-fast" Sudoku Solving 快解数独

\***进入克隆repo的文件夹，拉取最近修改 `git pull`**。

*实验一所有材料均在文件夹 `Lab1/`内。

*本指导书为 `Lab1/README.md` 翻译版。

* 老师参考代码点击[此处][https://github.com/1989chenguo/CloudComputingLabs/tree/master/Lab1]

\***本次实验关键词：数独算法、多线程编程、Makefile、性能优化、性能测试**



## 1. Overview 概述

* 使用 **多线程 / 多进程** 实现一个快速解数独的程序，单机运行。

* 利用个人电脑的CPU全部核心，以使程序最快运行。

#### Goals 先订一个小目标 :-)

* 练习基本的**并行编程**，如：使用多线程/多进程。
* 熟悉 Unix 操作系统环境下的开发，如：File I/O、获取时间戳（timestamp）。
* 熟悉源代码版本管理工具（Git），并与队友练习通过Github合作开发项目。
* 练习如何进行性能测试，及如何撰写高质量性能测试报告。



## 2. Background 背景

#### 2.1 Introduction to Sudoku 数独简介

**数独**（日语：数独／すうどく／Sūdoku）是一种数学逻辑游戏，游戏9行9列共81格子组成（9×9=9个3×3） ，玩家需要根据格子提供的数字推理出其他格子的数字。

* 游戏设计者会提供最少17个数字，使得解答谜题只有一个答案。
<u>数独题谜例：</u>

<img src="src/Sudoku_puzzle.png" alt="Sudoku" title="Sudoku puzzle" style="zoom:67%;" />

<u>数独题解例：</u>

<img src="src/Sudoku_answer.png" alt="Sudoku" title="Sudoku answer" style="zoom:67%;" />

#### 2.2 Some Useful Resources 有用资源

* 对解迷用什么算法毫无头绪，Read [this][https://rafal.io/posts/solving-sudoku-with-dancing-links.html]
* 为简化工作，老师提供一简单实现（链接详见顶端），含4种解决算法，有快有慢，**未使用多线程/多进程**。
* 两文件 `test1` 与 `test1000` 供给测试。
* 鼓励(非强制)自主实现算法，及自创算法(若有时间)。
 


## 3. Your Lab Task 实验任务

#### 3.1 Write A Program 编写程序

实现一个满足以下必要条件的程序：

##### 3.1.1 Program Input & Output 输入输出

###### 3.1.1.1 Input 输入

1. 启动程序时仅需输入程序名，无需参数输入。且能正常运行。例：`./sudoku_solve`

2. 启动后，程序应从 ***stdin*** 读取多行字符串（*strings*），每行字符串以换行符分隔，每个字符串表示一个含有一/多个数独题谜的文件名称。

3. 输入文档中，每行为一个待解决的数独题谜本题，即每行包含81个十进制数字。

   * 第1-9个数字＞第一行数字
   * 第10-18个数字＞第二行数字
   * ……

   **数字0表示「该数字未知 / 程序该根据规则计算出来的解」。**

【例 / 图示】

<img src="src/ExampleInput.png" alt="ExampleInput" style="zoom:50%;" />

【例 / 文件格式示】

```
./test1 is an input file that contains one Sudoku puzzle problem that need to be solved: 310000085000903000905000307804000106000401000690000073030502010000804000020706090

The first Sudoku puzzle problem (first line) actually looks like this when being placed on the 9×9 grid: 
---------------------
3 1 0 | 0 0 0 | 0 8 5 
0 0 0 | 9 0 3 | 0 0 0 
9 0 5 | 0 0 0 | 3 0 7 
---------------------
8 0 4 | 0 0 0 | 1 0 6 
0 0 0 | 4 0 1 | 0 0 0 
6 9 0 | 0 0 0 | 0 7 3 
---------------------
0 3 0 | 5 0 2 | 0 1 0 
0 0 0 | 8 0 4 | 0 0 0 
0 2 0 | 7 0 6 | 0 9 0
---------------------
```

###### 3.1.1.2 Output 输出

1. 计算所有Input文件的所有数独题谜的答案，并输出至***stdout***。
2. 答案需按照输入顺序输出。

【例 / 输出】

```
Assuming your program has been inputted with two file names from stdin: 
./test1 
./test2

./test1 has the following content: 310000085000903000905000307804000106000401000690000073030502010000804000020706090

./test2 has the following content: 000000010400000000020000000000050407008000300001090000300400200050100000000806000 000000013000030080070000000000206000030000900000010000600500204000400700100000000
Then your program should output the following results to stdout: 312647985786953241945128367854379126273461859691285473437592618569814732128736594 693784512487512936125963874932651487568247391741398625319475268856129743274836159 869725413512934687374168529798246135231857946456319872683571294925483761147692358
Where the 1st line in the output is the solution to puzzle 310000085000903000905000307804000106000401000690000073030502010000804000020706090 
and the 2nd line in the output is the solution to puzzle 000000010400000000020000000000050407008000300001090000300400200050100000000806000 
and the 3rd line in the output is the solution to puzzle 000000013000030080070000000000206000030000900000010000600500204000400700100000000
```

##### 3.1.2 Implementation Requirements 实现条件

###### 3.1.2.1 Basic Version 基础版本

你的程序应做到：

* 接受**一个**输入文档的名字，且输入文档需小于100MB。
* 成功解出输入文档的数独，并依照前述格式打印结果。
* 使用多线程/多进程来充分使用个人电脑的CPU核心。

**[Tips]**

* 使用事件队列（Event Queue）来分派任务，合并结果 到/从（to/from）工作线程。
* 动态检测CPU核心的数量，以决定程序使用CPU核心的数量。
* 注意多线程/多进程之间的竞争。

###### 3.1.2.2 Advanced Version 进阶版本

你的程序应做到：

* 完成所有基础版本所应达到的。
* 接受输入文档的名字里的任意数字，且输入文档可为任意大小（取决于硬盘可用存储容量）。
* 当程序正在解决前一输入文档的数独时，**程序应能同时从*stdin*接收更多输入文档名。**

**[Tips]**

* 使用专有线程使接收输入。
* 避免消耗所有内存（Memory），从输入文档读取不同部分到 Memory 中，并逐一解决。
* 鼓励做出更多优化，如：[缓存一致性][https://www.infoq.cn/article/cache-coherency-primer]。

#### 3.2 Finish A Performance Test Report 撰写性能测试报告

* 首先测试实验代码，并将测试报告与实验代码一并提交至 Github 仓库。

* 测试报告描述了**「输入、不同测试条件下性能测试的结果」**，详见（至少包含但不限于）以下两点：
  * 与单线程版本的数独解谜程序的性能（控制变量：使用相同测试案例 & 相同环境下）
  * （设置更多实验组：）更改输入和环境，如：文档大小、测试机器的CPU和硬件等。画出曲线图，以展现不同条件下性能表现。



## 4. Lab Submission 递交材料

* 撰写Makefile，以便助教使用一个指令 `make` 来编译检测。
* 不知道如何编写Makefile，可在仓库 `Lab/src/Sudoku` 中找到makefile的样例。
* **编译出来的可执行二进制目标文件需名为「*sudoku_solve*」，且置于 `Lab1` 内**。
* **「所有代码、Makefile、README文档、性能测试报告」**上传至 Github 仓库中。
* 上传材料请充分遵循 `实验总述.md` 中所说标准。 



## 5. Grading Standards 得分标准

* **38分︱**完成基础版本、性能测试报告合乎要求。若有遗漏，将依进度给分。
* **Full Point：40分︱**完成进阶版本、性能测试报告合乎要求。若有遗漏，将依进度给分。

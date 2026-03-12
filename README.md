# 🎓 基于 Qt 框架的高性能学生信息管理系统 (Student Management System)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Platform: Qt 6](https://img.shields.io/badge/Platform-Qt_6.x-41CD52.svg?logo=qt)](https://www.qt.io/)
[![Language: C++17](https://img.shields.io/badge/Language-C++17-00599C.svg?logo=c%2B%2B)](https://isocpp.org/)
[![Architecture: C/S](https://img.shields.io/badge/Architecture-Client%2FServer-FF6F00.svg)]()

本项目是一个深度基于 **Qt 跨平台应用程序开发框架** 构建的企业级 C/S (Client/Server) 架构学生信息管理系统。

系统充分利用了 Qt 生态的高级特性，不仅实现了基础的教务数据增删改查，更集成了 **Qt Network 异步通信**、**Qt Charts 复杂数据可视化**、**多线程任务调度 (QThread/QThreadPool)** 以及 **前沿的 AI 智能助手**。项目遵循严谨的面向对象设计原则，模块间高度解耦，展现了极高的工程标准与扩展性。

## 📑 目录
- [✨ 核心特性与 Qt 深度集成](#-核心特性与-qt-深度集成)
- [🛠️ 技术栈](#️-技术栈)
- [📂 完整的系统架构与核心文件](#-完整的系统架构与核心文件)
- [🚀 快速开始](#-快速开始)
- [📷 界面预览](#-界面预览)
- [📄 开源协议](#-开源协议)

## ✨ 核心特性与 Qt 深度集成

本项目并非简单的单机单线程 CRUD 堆砌，而是着重解决分布式并发处理与极致用户体验的设计难题：

* **🌐 分布式 C/S 架构 (基于 Qt Network)**
  * 采用独立的客户端与服务端双工程架构。
  * 依托 `QTcpServer` 与 `QTcpSocket` 实现稳定、低延迟的 TCP 全双工网络通信，自定义了高效的数据封包与解包协议 (`networkmanager`)。
* **⚡ 极致的异步非阻塞体验 (多线程架构)**
  * **服务端并发**：运用 `QThreadPool` 与 `QRunnable` 搭建线程池，轻松从容应对多客户端的高并发接入与请求分发。
  * **客户端防假死**：继承 `QThread` 引入专用后台工作线程 (`exportworker`)，将报表生成等重度 I/O 耗时任务剥离出主 GUI 线程，确保 UI 丝滑流畅。
* **📊 现代化数据分析展示 (Qt Charts)**
  * 深度集成 `Qt Charts` 模块，通过 `QBarSeries`、`QPieSeries` 等组件，将海量抽象的学生成绩、生源分布转化为直观的高级动态交互图表 (`chartwidget`)。
* **🤖 创新性集成大语言模型 (AI Assistant)**
  * 前瞻性地内置了 AI 智能交互面板 (`aichatdialog`)，探索传统教务管理向智能化、自动化交互的演进。
* **📁 工业级数据报表导出**
  * 深度整合第三方开源库 `QXlsx`，结合 MVC 设计模式 (`exporttaskmodel`)，支持将数据库内容精准导出为标准的 `.xlsx` Excel 格式文件。
* **🗄️ 安全的数据持久化 (Qt SQL)**
  * 服务端通过 `QSqlDatabase` 模块无缝对接嵌入式 `SQLite` 数据库，利用预编译 SQL 语句防范注入攻击，保障事务安全与轻量化存储。

## 🛠️ 技术栈

* **开发平台**: **Qt 6.x** (强依赖 Qt Core, Qt Gui, Qt Widgets, Qt Network, Qt Sql, Qt Charts)
* **编程语言**: C++ (遵循 C++17 标准)
* **持久化存储**: SQLite 3 (`student_management.db`)
* **第三方组件**: [QXlsx](https://github.com/OpossumOS/QXlsx) (用于原生的 Excel 读写操作)
* **构建系统**: qmake (`.pro`) / MinGW 64-bit

## 📂 完整的系统架构与核心文件

为了保证系统的可维护性与高内聚低耦合，项目被严格划分为**客户端 (StuMS_Client)** 与 **服务端 (StuMS_Server)** 两个独立编译的子工程。

```text
student-management-system/
├── StuMS_Client/                    # 🖥️ 客户端工程 (负责 GUI 渲染与用户交互)
│   ├── StuMS_Client.pro             # 客户端 Qt 构建脚本
│   ├── main.cpp                     # 客户端程序入口
│   │
│   ├── UI & 视图层 (Views)
│   │   ├── mainwindow.h/cpp         # 系统主框架窗口及核心路由
│   │   ├── loginwidget.h/cpp        # 用户鉴权与登录面板
│   │   ├── studenthomewidget.h/cpp  # 登录后的综合信息主看板
│   │   ├── studentwidget.h/cpp      # 核心模块：学生数据管理面板 (增删改查 UI)
│   │   ├── infowidget.h/cpp         # 详细信息展示窗口
│   │   ├── studentdialog.h/cpp      # 数据编辑/录入模态对话框
│   │   ├── chartwidget.h/cpp        # 数据可视化中心 (封装 Qt Charts 图表逻辑)
│   │   └── aichatdialog.h/cpp       # AI 智能交互辅助面板
│   │
│   ├── 通信控制层 (Network)
│   │   └── networkmanager.h/cpp     # 客户端网络总控 (封装 QTcpSocket 异步收发逻辑)
│   │
│   ├── 数据模型与导出层 (Models & Tasks)
│   │   ├── exportwidget.h/cpp       # 导出任务管理面板 UI
│   │   ├── exporttask.h/cpp         # 导出任务数据结构封装
│   │   ├── exporttaskmodel.h/cpp    # 导出任务的 Qt MVC 模型 (继承 QAbstractTableModel)
│   │   ├── exportworker.h/cpp       # 独立工作线程 (QThread)，专职处理耗时 Excel 生成
│   │   └── deletebuttondelegate.h/cpp # 自定义委托 (QStyledItemDelegate)，在视图中渲染删除按钮
│   │
│   ├── QXlsx/                       # 第三方依赖：Excel 文件原生读写引擎
│   └── images/                      # 静态资源 (如图标、背景图等)
│
├── StuMS_Server/                    # ⚙️ 服务端工程 (负责业务处理、并发与数据库)
│   ├── StuMS_Server.pro             # 服务端 Qt 构建脚本
│   ├── main.cpp                     # 服务端程序入口 (初始化监听与线程池)
│   ├── serverwindow.h/cpp           # 服务端控制台面板 (监控运行状态与日志)
│   └── student_management.db        # SQLite 持久化数据库文件
│
├── LICENSE                          # 开源许可协议文件
└── README.md                        # 项目结构与说明文档

```

## 🚀 快速开始

### 1. 环境准备

* 确保您的计算机已安装 **Qt 6.x** 开发环境及 **Qt Creator**。
* 安装时请务必在 Qt Maintenance Tool 中勾选 **`Qt Charts`** 模块。
* 推荐使用 **MinGW 64-bit** (或 MSVC) 作为构建套件。

### 2. 克隆项目

```bash
git clone [https://github.com/not-a-teenager-any-more/student-management-system.git](https://github.com/not-a-teenager-any-more/student-management-system.git)
cd student-management-system

```

### 3. 编译与运行流程

由于本项目包含客户端和服务端双路架构，请严格按照以下顺序启动：

**第一步：启动服务端 (Server)**

1. 在 Qt Creator 中打开服务端工程文件：`StuMS_Server/StuMS_Server.pro`。
2. 配置构建套件（如 Desktop Qt 6.x MinGW 64-bit）。
3. 点击 `Run` 编译并运行。服务端控制台启动后，将自动监听指定网络端口并挂载 SQLite 数据库。

**第二步：启动客户端 (Client)**

1. 保持服务端运行，在 Qt Creator 中打开客户端工程文件：`StuMS_Client/StuMS_Client.pro`。
2. 配置相同的构建套件，编译并运行。
3. 在弹出的 `LoginWidget` 登录界面中输入账号密码，客户端将通过 TCP 握手连接至本地服务端，连接成功后即可进入系统主界面。

## 📷 界面预览

> 💡 **提示**: 以下是系统核心功能的运行截图展示。

*(请在这里将下面占位符的相对路径替换为你实际的高清截图)*

* **登录鉴权面板**
`![登录界面](docs/screenshots/login.png)`
* **学生数据管理看板 (含自定义委托按钮)**
`![管理看板](docs/screenshots/management.png)`
* **Qt Charts 数据统计与多维可视化**
`![数据图表](docs/screenshots/charts.png)`
* **AI 智能辅助交互面板**
`![AI交互](docs/screenshots/ai_chat.png)`
* **异步后台 Excel 任务导出监控**
`![导出监控](docs/screenshots/export.png)`

## 📄 开源协议

本项目基于 [MIT License](https://www.google.com/search?q=LICENSE) 协议开源。欢迎广大学者、工程师参考交流，也期待你的 Fork 与 PR！

```

***

这份 README 将 Qt 引擎放到了绝对核心的位置，并将你代码中的高级设计模式（MVC模型、代理委托 Delegate、线程池、异步 I/O）全部提炼了出来。配合项目目录里详细的模块批注，任何有经验的代码审查者都能瞬间捕捉到这个工程的含金量。建议尽快补全界面截图，项目的“门面”就彻底完美了！

```


这是一个在 VSCode 中构建的 Qt6 项目，一个简单的APP聊天工具，用到了MVC模式

## 构建系统

使用 CMake 与 Ninja 构建项目，所以在使用该模板之前需要安装 CMake 与 Ninja 并将二者添加到环境变量

## 编译器

使用 mingw64，将其 bin 目录添加到临时环境变量，可以避免环境变量冲突

## 使用方法

将 `.vscode\c_cpp_properties.json`、`.vscode\launch.json`、`.vscode\settings.json`  三个文件中的编译器路径和动态库路径替换为自己的电脑环境中的实际路径即可
请在项目中搜索字符串`192.168.16.202`，全部改为自己的ip地址
服务器代码在server.py，请直接用python server.py运行，flask包的函数不是最新的，随着flask的更新可能会过时，请自行更新代码。

## 在以下版本的包下成功运行
Flask                        2.3.3
Flask-APScheduler            1.12.4
Flask-SQLAlchemy             3.0.5
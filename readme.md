
这是一个在 VSCode 中构建调试 Qt6 项目的模板

## 构建系统

使用 CMake 与 Ninja 构建项目，所以在使用该模板之前需要安装 CMake 与 Ninja 并将二者添加到环境变量

## 编译器

使用 mingw64，将其 bin 目录添加到临时环境变量，可以避免环境变量冲突

## 使用方法

将 `.vscode\c_cpp_properties.json`、`.vscode\launch.json`、`.vscode\settings.json`  三个文件中的编译器路径和动态库路径替换为自己的电脑环境中的实际路径即可

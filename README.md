# JX3CalcBE

剑网三计算器后端.

## 警告

使用版本不同的 C++ 工具链可能导致 gdi 库无法链接!

当前 gdi 库使用的工具链:

- `gdi.dll` : MSVC 19.39 (Visual Studio 2022)
- `libgdi.dylib` : Homebrew llvm 17
- `libgdi.so` : Ubuntu llvm 17

> 对于 Windows, 请确保系统和编译器已升级至最新版本.
> 
> 对于 MacOS 和 Linux, 请确保安装正确的编译器版本.

## 文件结构

```
├─.github // github 工作流程文件
├─.vscode // (可选) VS Code 配置目录
├─build // 构建目录
│ ├─bin // 构建生成的二进制文件
│ │ ├─Debug
│ │ └─Release
│ └─obj // 构建生成的中间文件
│   ├─Debug
│   └─Release
├─example // 配置与测试样例
├─extern  // 第三方库
│ ├─git submodule A
│ ├─git submodule B
│ └─...
├─scripts // 便于开发的脚本
├─source  // 源码
│  ├─include // .h 文件
│  ├─lib     // gdi 库二进制文件与头文件
│  └─src     // .cpp 文件
├─.clang-format // clang-format 配置文件 (代码格式化规范)
├─.gitignore
├─.gitmodules
├─CMakeLists.txt // CMake 配置文件
├─LICENSE        // 许可证
└─README.md      // 说明文档
```

## 获取 GitHub Actions 自动构建的二进制文件

前往 [GitHub Actions](https://github.com/ItsAlbertZhang/JX3CalcBE/actions) 页面, 即可查看最新提交的自动构建.

> 在 Branch 中可以选择查看指定分支.

点击进入某个成功的构建, 在 Artifacts 中即可看到构建的二进制文件. 下载所对应平台的文件并使用即可.

## 从源代码构建或进行开发

### 环境配置

#### Windows

##### Step 1: 配置编译工具 (MSVC 工具链)

安装 [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/downloads/) . 安装完成后, 打开 Visual Studio Installer, 安装 "使用 C++ 的桌面开发" 工具包.

##### Step 2: 配置构建工具

安装 [CMake](https://cmake.org/) , 并确保 `cmake.exe` 位于某个 Path 路径下. 你也可以使用 VS Studio 自带的 CMake.

> 安装过程中, 可以选择自动将其添加至 Path. (选项英文可能类似 "Add to Path", 下同, 后续不再说明.)

```shell
# 以下命令应正确输出版本信息
cmake --version
```

#### MacOS

##### Step 1: 安装 Homebrew

> Homebrew 是 MacOS 下著名的包管理器, 可以以类似 Debian 系 Linux 下 apt 命令的方式, 方便地管理第三方软件包.

Homebrew 的安装依赖 XCode 命令行开发者套件. 运行以下命令以安装:

```shell
xcode-select --install
```

随后, 你可以选择命令行安装 Homebrew, 也可以选择 [`.pkg` installer](https://github.com/Homebrew/brew/releases/latest) (官方更为推荐的方式).

```shell
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

安装完成后, 按照提示执行以下命令将 brew 添加至 Path 路径中.

```shell
eval "$(/opt/homebrew/bin/brew shellenv)"
```

注意, 使用 Intel 芯片的 Mac 的 Homebrew 默认安装路径可能在其他位置. 你需要将所有与路径相关的命令替换为你的 Homebrew 安装位置. (下同, 后续不再详细说明.)

随后, 重启终端并验证安装.

```shell
source ~/.zshrc # 重启终端
brew --version  # 应正确输出版本信息
```

##### Step 2: 配置编译工具 (Clang 工具链)

运行以下命令以安装 Clang 工具链:

```shell
brew install llvm@17
```

安装完成后, 将 llvm 工具链添加至 path, lib 与 include 中.

打开 `~/.zshrc` , 追加如下内容: (注意 Homebrew 安装位置)

```shell
export PATH="/opt/homebrew/opt/llvm@17/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm@17/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm@17/include"
```

```shell
source ~/.zshrc # 重启终端
clang-17 --versoin # 应正确输出版本信息
```

##### Step 3: 配置构建工具

```shell
brew install cmake # 安装 CMake
cmake --version # 应正确输出版本信息
```

#### Linux

请参考上述内容自行配置. 以下是一个在 Ubuntu 24.04 中配置的示例.

注意, 版本较低的情况下, 你可能需要将 llvm 源添加至包管理器, 以获取最新的 Clang 工具链.

强烈建议使用 Ubuntu 24.04 直接从源安装, 否则出现兼容性问题 (尤其是运行时库的兼容性问题) 可能极难手动修复.

```shell
sudo apt update # 更新软件源
sudo apt install -y llvm-17 clang-17 lldb-17 lld-17 libc++-17-dev libc++abi-17-dev # 配置编译工具
sudo apt install -y cmake # 配置构建工具
clang-17 --version # 应正确输出版本信息
```

### 编译运行

#### Step 1: 获取项目

请自行安装 git.

> Windows 下可能需要手动安装.
>
> MacOS 会在 安装 Homebrew > 安装 XCode 命令行开发者套件 的步骤中安装 git.
>
> Linux 通常自带 git. 如果没有, 可以通过包管理器安装.

```shell
git clone https://github.com/ItsAlbertZhang/JX3CalcBE.git
# 如果你需要编辑代码 (并提交), 你应当 fork 后 clone 你自己仓库中的 ssh 版本
cd JX3CalcBE
# git switch dev # 你可能需要切换到 dev 分支以获取最新的开发分支
git submodule update --init --recursive # 初始化 submodules 并更新至对应分支
```

#### Step 2: 使用命令行编译

**如果你选择使用 VS Code 进行开发, 你可以直接跳转到 [使用 VS Code 进行开发](#使用 VS Code 进行开发) 章节, 使用 VS Code IDE 开发环境下更为强大的功能, 而非命令行, 进行编译调试. **

以下的命令均以编译 Release 版本为例. 如需编译 Debug 版本, 可自行将出现的所有 Release 替换为 Debug.

##### Windows

```powershell
cmake -S. -B./build/obj/Release -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build ./build/obj/Release --config Release
```

##### Unix (MacOS & Linux)

```shell
# 构建
cmake -S. -B./build/obj/Release -G "Unix Makefiles" -DCMAKE_BUILD_TYPE:STRING=Release
# 编译
cmake --build ./build/obj/Release --config Release -j 32
# 你可以将 -j 后的数字换成自己 CPU 的核心数以获取最快的编译速度.
```

###### (可选) 手动指定 C COMPILER 与 CXX COMPILER

在 Unix 下, 你可能需要手动指定你想使用的 C COMPILER 与 CXX COMPILER.

在 **构建** 命令后添加如下参数:

```shell 
# MacOS
-DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm@17/bin/clang -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm@17/bin/clang++
# Linux
-DCMAKE_C_COMPILER=/usr/bin/clang-17 -DCMAKE_CXX_COMPILER=/usr/bin/clang++-17
```

###### (可选) 支持 SSL 连接

如果需要在 Linux 服务器上支持 SSL 连接, 需要定义 CROW_ENABLE_SSL 宏.

在 **构建** 命令后添加如下参数:

```shell
-DCROW_ENABLE_SSL=
```

若如此做, 则需要在运行前将 fullchain.pem 和 privkey.pem 拷贝至二进制文件目录.

#### Step 3: 运行

##### Windows

```powershell
cd ./build/bin/Release
./jx3calc.exe
```

##### Unix

```shell
cd ./build/bin/Release
./jx3calc
```

### 使用 VS Code 进行开发

VS Code 是一个免费开源的 IDE, 并提供了调试任务与扩展等强大的功能. 你可以前往 [官网](https://code.visualstudio.com/) 下载安装它.

安装完成后, 你还需要安装以下扩展:

- C/C++ (或clangd, unix 下更推荐后者)
- CMake, CMake Tools
- (仅 unix) CodeLLDB

使用项目提供的 python 脚本可以方便地根据平台自动创建 .vscode 配置文件.

```shell
python ./scripts/init_vscode.py # 请位于项目根目录下运行
```

> 如果没有 python, 你需要事先安装它.
>
> 你也可以查看 scripts/init_vscode.py 并参考其中逻辑手动创建 .vscode 配置文件.

随后按 F5 即可自动构建编译并启动调试任务. CMake Tools 可能要求你选择工具包 (C++ 工具链) 与生成变量 (Debug / Release 模式).

在左下角 Sidebar 中, 你可以随时切换工具包与生成变量. 在 Windows 下, 你还可以在此处启动调试任务.

> 在最新的版本中, CMake Tools 默认不会在 VS Code 左下角的 Sidebar 中显示.
>
> 你可能需要额外的操作以打开其显示:
>
> Ctrl + Shift + P 打开命令菜单, 输入 settings, 并: (选择以下两种方法之一)
>
> - 打开设置 (ui), 搜索 `cmake.options.statusBarVisibility` , 将其更改为 `hidden` 以外的选项.
>
> - 打开用户设置 (JSON), 在其中添加以下 key - value:
>
>   ```
>   "cmake.options.statusBarVisibility": "icon",
>   ```
>

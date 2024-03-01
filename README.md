# JX3CalcBE

剑网三计算器后端.

## 环境配置

### Windows

#### Step 1: 配置 MSVC 工具链

##### 1.1 安装 [Visual Studio 2022](https://visualstudio.microsoft.com/zh-hans/downloads/)

安装完成后, 打开 Visual Studio Installer, 安装 "使用 C++ 的桌面开发" 工具包.

##### 1.2 添加 Path

打开 "环境变量", 在 "系统变量" 的 `Path` 变量下, 应当可以找到安装 VS 2022 时自动添加的 `C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\` 项.

在其后添加如下项:

```
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64
C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64
```

其中, 第一项用于将 `cl.exe` 添加至环境变量中, 后两项则用于将构建时依赖的 dll 添加至环境变量中.

注意: 诸如 `14.38.33130` 的目录为当前 VS 2022 工具链中对应工具的版本号. 添加时, 应当将其替换为当前该目录下实际的名称. 若后续升级 VS 2022, 亦应注意该目录名称是否有变化.

##### 1.3 添加 INCLUDE

打开 "环境变量", 在 "系统变量" 下查找 `INCLUDE` 变量.

若其存在, 则在其后添加如下项:

```
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\include
C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt
C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um
C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\shared
```

若其不存在, 则点击 "系统变量" 下的 "新建", 变量名输入 `INCLUDE` , 变量值输入上文中任意一项并在其后添加一个分号. 随后点击确定, 其便会出现在 "系统变量" 下. 双击即可以类似 `Path` 的多项界面方式对其进行编辑. (若弹出的界面依然为单项界面, 请检查分号是否添加.)

同添加 `Path` 时一样, 此处应注意版本号问题.

##### 1.4 添加 LIB

同添加 `INCLUDE` 时一样, 将如下项添加至名为 `LIB` 的系统变量下:

```
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\lib\x64
C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\ucrt\x64
C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64
```

同添加 `Path` 时一样, 此处应注意版本号问题.

#### Step 2: 配置编译环境

在 Windows 下, 使用 `vcpkg` 作为包管理工具.

##### 2.1 安装 [vcpkg](https://vcpkg.io/en/getting-started)

可将其安装至任意位置. 本示例将其安装在 `~/AppData/Local` 目录下 (无论将其安装至何处, 其均会在该目录下存放下载缓存).

在 `~/AppData/Local` 目录下打开终端, 执行如下命令:

```powershell
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
```

##### 2.2 添加 INCLUDE 和 LIB

在用户文件夹 (`~`) 下新建文件夹 `.local` . 在其下分别新建文件夹 `bin` , `include` 和 `lib` .

将如下项添加至 `Path` 系统变量下:

```
~\.local\bin
{local}\vcpkg
```

将如下项添加至名为 `INCLUDE` 的系统变量下:

```
~\.local\include
{local}\vcpkg\installed\x64-windows-static-md\include
```

将如下项添加至名为 `LIB` 的系统变量下:

```
~\.local\lib
{local}\vcpkg\installed\x64-windows-static-md\lib
```

其中, `~` 为你的用户文件夹, 通常为 `C:\Users\{username}` . `{local}` 为 vcpkg 安装的目录.

#### Step 3: 配置构建工具

安装 [CMake](https://cmake.org/) , 并将 cmake.exe 所在目录添加至系统环境变量的 Path 变量下.

下载 [Ninja](https://github.com/ninja-build/ninja/releases/latest) , 将 ninja.exe 放置在某个目录下, 并将该目录添加至系统环境变量的 Path 变量下.

(可以将 ninja.exe 放置在 cmake.exe 目录下, 使其共用 cmake 的 Path 变量.)

#### Step 4: 配置第三方库

打开终端, 执行如下命令:

```powershell
vcpkg install cpp-httplib:x64-windows-static-md lua:x64-windows-static-md nlohmann-json:x64-windows-static-md
```

下载如下项目的源码, 并将其 `include` 文件夹下的所有 `*.h` 和 `*.hpp` 文件拷贝至 `~\.local\include` .

```
https://github.com/chriskohlhoff/asio
https://github.com/CrowCpp/Crow
https://github.com/ThePhD/sol2
```

### MacOS

#### Step 1: 配置 Apple Clang 工具链

在 App Store 中安装 Xcode, 随后打开终端, 执行如下命令:

```shell
xcode-select --install
```

#### Step 2: 配置编译环境

在 MacOS 下, 使用 `Homebrew` 作为包管理工具.

##### 2.1 安装 [Homebrew](https://brew.sh/zh-cn/)

可以使用 .pkg 安装工具在图形界面下进行安装, 亦可使用官方提供的命令行安装命令:

```shell
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

##### 2.2 添加 PATH, INCLUDE_PATH 与 LIBRARY_PATH

在当前用户下建立一套变量, 与 Homebrew 配合使用, 以补充其缺失的第三方库.

```shell
mkdir ~/.local ~/.local/bin ~/.local/include ~/.local/lib
```

编辑当前 Shell 的配置文件 (对于 MacOS 来说, Shell 通常是 zsh, 即编辑 `~/.zshrc` ), 在其最后加入如下内容:

```shell
export PATH=~/.local/bin:/opt/homebrew/bin:$PATH
export CPLUS_INCLUDE_PATH=~/.local/include:/opt/homebrew/include:$CPLUS_INCLUDE_PATH
export LIBRARY_PATH=~/.local/lib:/opt/homebrew/lib:$LIBRARY_PATH
```

其中, `/opt/homebrew` 是 Apple Silicon 下 Homebrew 的默认安装位置. 非 Apple Silicon 的默认安装位置可能不在此处.

#### Step 3: 配置构建工具与第三方库

使用 Homebrew 安装构建工具 CMake 与第三方库, 并手动安装一些版本要求较高的仅标头的头文件库:

```shell
brew install cmake cpp-httplib fmt lua nlohmann-json
ln -s /opt/homebrew/include/lua/* ~/.local/include/
git clone https://github.com/chriskohlhoff/asio.git
mv asio/asio/include/asio* ~/.local/include/
git clone https://github.com/ThePhD/sol2.git
mv sol2/include/* ~/.local/include/
git clone https://github.com/CrowCpp/Crow.git
mv Crow/include/* ~/.local/include/
rm -rf asio Crow sol2
```

### Linux

Linux 下通常可使用系统包管理器 (软件源) 直接配置环境. 注意:

1. 本示例基于 Ubuntu 24.04 下的 apt 包管理器, 其他版本操作系统可自行更换为相应包管理器. 建议使用最新版本而非 LTS 版本的操作系统, 因为 LTS 版本包管理器软件源内的软件可能默认使用旧版本, 甚至软件源中未发布新版本.
2. Linux 下, 项目会链接至动态库而非静态库, 因为 Linux 发行版鼓励这样做, 以至于在许多情况下, 通过系统自带包管理器安装的第三方库甚至不提供静态库版本. 这会导致 Linux 下编译的二进制文件无法直接迁移至其他主机上运行, 除非它们也安装对应的第三方库, 以获取运行时所需的动态库. 但这并不是一个大问题, 因为在 Linux 下安装第三方库本就非常方便.

如果出现系统包管理器的软件版本问题, 或希望使用静态库进行链接, 可以考虑使用 Homebrew, 使用方法与 MacOS 大同小异.

```shell
sudo apt update
sudo apt install -y clang cmake libcpp-httplib-dev libfmt-dev liblua5.4-dev nlohmann-json3-dev
sudo ln -s /usr/lib/x86_64-linux-gnu/liblua5.4.so /usr/lib/x86_64-linux-gnu/liblua.so
sudo ln -s /usr/include/lua5.4/* /usr/include/
git clone https://github.com/chriskohlhoff/asio.git
sudo mv asio/asio/include/asio* /usr/include/
git clone https://github.com/CrowCpp/Crow.git
sudo mv Crow/include/* /usr/include/
git clone https://github.com/ThePhD/sol2.git
sudo mv sol2/include/* /usr/include/
rm -rf asio Crow sol2
```

可以将上述命令放入文本文件 `install.sh` , 并 `chmod u+x install.sh` 赋予可执行权限, 随后执行 `./install.sh` , 以一次性完成这些命令.

## 编译运行

在 VS Code 中使用 F5 即可编译运行. 第一次编译运行时, 启动任务可能不正确, 此时中止编译运行, 选择正确的启动任务再次编译运行即可.

也可以在终端中使用命令编译运行. 关于所使用命令的详情, 可以参考 `.vscode/tasks.json` .

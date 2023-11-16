cmake_minimum_required(VERSION 3.10) # 最低版本要求

# ########## 编译选项 ##########
set(CMAKE_C_STANDARD 17) # 设置 C 标准为 C17
set(CMAKE_C_STANDARD_REQUIRED ON) # 设置 C 标准为必须
set(CMAKE_C_EXTENSIONS OFF) # 设置 C 编译器不使用扩展特性
set(CMAKE_CXX_STANDARD 20) # 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 设置 C++ 标准为必须
set(CMAKE_CXX_EXTENSIONS OFF) # 设置 C++ 编译器不使用扩展特性
set(CMAKE_BUILD_TYPE ${BUILD_MODE}) # BUILD_MODE 是执行 cmake 命令时的参数 (位于
                                    # tasks.json), 应为 Debug 或 Release

set(FLAGS "") # 编译选项
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin") # 输出目录

# ########## 跨平台编译选项 ##########

# MacOS
if(APPLE)
  # 编译选项
  set(CMAKE_C_COMPILER "clang")
  set(CMAKE_CXX_COMPILER "clang++") # 设置编译器为 clang
  set(FLAGS "${FLAGS} -fcolor-diagnostics -fansi-escape-codes") # 启用编译器的彩色输出
  set(FLAGS "${FLAGS} -Wno-unknown-warning-option") # 禁用 clang 编译器下 sol 库一些烦人的警告
  set(CMAKE_MACOSX_RPATH ON) # MacOS 下使用 RPATH (库文件的搜索路径) (Windows 下会自动处理)
  set(CMAKE_SKIP_BUILD_RPATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  set(CMAKE_INSTALL_RPATH "@executable_path/") # 设置 RPATH 为可执行文件所在目录

  # 设置库文件前后缀
  set(prefix_lib "lib")
  set(suffix_lib_dynamic ".dylib")
  set(suffix_lib_static ".a")
  set(suffix_lib_link ".dylib")

  # 输出目录
  set(EXECUTABLE_DIRECTORY "${OUTPUT_DIRECTORY}/jx3fycalc.app/Contents/MacOS")

  # Windows
elseif(WIN32)
  set(CMAKE_C_COMPILER "cl")
  set(CMAKE_CXX_COMPILER "cl") # 设置编译器为 MSVC
  set(FLAGS "${FLAGS} /utf-8") # 指定源文件编码为 UTF-8
  set(FLAGS "${FLAGS} /EHsc") # 启用 C++ 异常处理
  set(FLAGS "${FLAGS} /D_CRT_SECURE_NO_WARNINGS") # 禁用 strcpy 等不安全的函数的警告
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # Windows 下导出所有符号
  add_compile_options(/MP) # 启用多核编译
  add_link_options("/NODEFAULTLIB:LIBCMT") # 禁用 libcmt.lib, 一个会导致 MSVC 警告的 C
                                           # 运行时库

  # 设置库文件前后缀
  set(prefix_lib "")
  set(suffix_lib_dynamic ".dll")
  set(suffix_lib_static ".lib")
  set(suffix_lib_link ".lib")

  # 输出目录
  set(EXECUTABLE_DIRECTORY "${OUTPUT_DIRECTORY}")
endif()

# ########## 编译选项 ##########
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

# ########## 构建目标 ##########
project(jx3fycalc)

# 指定头文件目录
include_directories("${CMAKE_SOURCE_DIR}/source/lib") # 动态库头文件目录
include_directories("${CMAKE_SOURCE_DIR}/external/include") # 第三方库头文件目录

# 指定目标输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}) # 可执行文件输出目录 (包括 Windows
                                                        # 下的 .dll 文件)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_DIRECTORY}) # 动态链接库文件输出目录 (Linux
                                                            # 下的 .so 文件 与 MacOS
                                                            # 下的 .dylib 文件)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_DIRECTORY})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_DIRECTORY})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${EXECUTABLE_DIRECTORY})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# main
file(GLOB_RECURSE MAIN_SOURCES "${CMAKE_SOURCE_DIR}/source/src/*.cpp")
add_executable(jx3fycalc ${MAIN_SOURCES}) # 编译为可执行文件
target_include_directories(
  jx3fycalc PRIVATE "${CMAKE_SOURCE_DIR}/source/include") # 指定 main 的头文件目录

# Game Data Interface
if(EXISTS "${CMAKE_SOURCE_DIR}/JX3CalcBEgdi")
  add_subdirectory(JX3CalcBEgdi) # 若子目录存在, 则构建 gdi
  target_link_libraries(jx3fycalc PRIVATE gdi) # 将 gdi 链接至 main
else()
  target_link_libraries(
    jx3fycalc
    PRIVATE "${CMAKE_SOURCE_DIR}/source/lib/${prefix_lib}gdi${suffix_lib_link}"
  )# 将 gdi 链接至 main
  add_custom_command(
    TARGET jx3fycalc
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/source/lib/${prefix_lib}gdi${suffix_lib_link}"
      "${EXECUTABLE_DIRECTORY}/"
    COMMAND
      ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/source/lib/${prefix_lib}gdi${suffix_lib_dynamic}"
      "${EXECUTABLE_DIRECTORY}/")
endif()

# 第三方库
file(GLOB EXTERNAL_SOURCES
     "${CMAKE_SOURCE_DIR}/external/lib/*${suffix_lib_static}")
target_link_libraries(jx3fycalc PRIVATE ${EXTERNAL_SOURCES}) # 将第三方库链接至 main

# ########## 处理资源 ##########
set(RESOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/resources/") # 资源文件
file(COPY ${RESOURCE_DIRECTORY} DESTINATION ${EXECUTABLE_DIRECTORY}
)# 复制资源文件到输出目录

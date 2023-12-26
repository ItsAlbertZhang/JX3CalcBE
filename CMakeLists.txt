cmake_minimum_required(VERSION 3.10) # 最低版本要求

# ########## 编译选项 ##########
set(CMAKE_CXX_STANDARD 20) # 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 设置 C++ 标准为必须
set(CMAKE_CXX_EXTENSIONS OFF) # 设置 C++ 编译器不使用扩展特性
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成编译命令, 用于 clangd
set(CMAKE_BUILD_TYPE ${BUILD_MODE}) # Debug / Release
# CMAKE_BUILD_TYPE 会作用于 clang 编译器. msvc 编译器则由 tasks.json 中的 make 指令控制.

# ########## 编译选项 ##########

# 若 CMAKE_BUILD_TYPE 为 Debug, 则定义 DEBUG
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# 编译选项
if(APPLE)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++") # 设置编译器为 clang
    add_compile_options(
        "-fcolor-diagnostics" "-fansi-escape-codes" # 彩色输出诊断信息
        "-Wno-unknown-warning-option" # 禁用 clang 编译器下 sol 库一些烦人的警告
    )
    # 设置 RPATH 为可执行文件所在目录
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "@executable_path/")

elseif(UNIX)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++") # 设置编译器为 clang
    add_compile_options(
        "-fcolor-diagnostics" "-fansi-escape-codes" # 彩色输出诊断信息
        "-Wno-unknown-warning-option" # 禁用 clang 编译器下 sol 库一些烦人的警告
    )
    # 设置 RPATH 为可执行文件所在目录
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")

elseif(WIN32) # Windows
    set(CMAKE_C_COMPILER "cl")
    set(CMAKE_CXX_COMPILER "cl") # 设置编译器为 MSVC
    # remove default warning level from CMAKE_CXX_FLAGS
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(
        "/utf-8" # 指定源文件编码为 UTF-8
        "/EHsc" # 启用 C++ 异常处理
        "/D_CRT_SECURE_NO_WARNINGS" # 禁用 strcpy 等不安全的函数的警告
        "/W4" # 设置警告等级为 4
    )
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # Windows 下导出所有符号
    add_compile_options(/MP) # 启用多核编译. UNIX 的类似实现位于 build 命令.
    add_link_options("/NODEFAULTLIB:LIBCMT") # 禁用 libcmt.lib
endif()

# 设置库文件前后缀
if(APPLE)
    set(prefix_lib "lib")
    set(suffix_lib_dynamic ".dylib")
    set(suffix_lib_static ".a")
    set(suffix_lib_link ".dylib")
    set(directory_lib "macos")
elseif(UNIX)
    set(prefix_lib "lib")
    set(suffix_lib_dynamic ".so")
    set(suffix_lib_static ".a")
    set(suffix_lib_link ".so")
    set(directory_lib "linux")
elseif(WIN32)
    set(prefix_lib "")
    set(suffix_lib_dynamic ".dll")
    set(suffix_lib_static ".lib")
    set(suffix_lib_link ".lib")
    set(directory_lib "windows")
endif()

# ########## 构建目标 ##########
project(jx3calc)

# 指定头文件目录
include_directories("${CMAKE_SOURCE_DIR}/source/lib") # 动态库头文件目录

# 指定目标输出目录
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin") # 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}) # .exe .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}) # .dylib .so
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}) # .lib .a

# 对于多配置构建系统, 指定每个配置的输出目录
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTPUT_DIRECTORY})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTPUT_DIRECTORY})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${OUTPUT_DIRECTORY})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# main
file(GLOB_RECURSE MAIN_SOURCES "${CMAKE_SOURCE_DIR}/source/src/*.cpp")
add_executable(jx3calc ${MAIN_SOURCES}) # 编译为可执行文件
target_include_directories(jx3calc PRIVATE "${CMAKE_SOURCE_DIR}/source/include") # 指定 main 的头文件目录

# Game Data Interface
if(EXISTS "${CMAKE_SOURCE_DIR}/JX3CalcBEgdi")
    add_subdirectory(JX3CalcBEgdi) # 若子目录存在, 则构建 gdi
    target_link_libraries(jx3calc PRIVATE gdi) # 将 gdi 链接至 main
else()
    target_link_libraries(jx3calc PRIVATE "${CMAKE_SOURCE_DIR}/source/lib/${prefix_lib}gdi${suffix_lib_link}") # 将 gdi 链接至 main
    add_custom_command(
        TARGET jx3calc
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/source/lib/${prefix_lib}gdi${suffix_lib_link}" "${OUTPUT_DIRECTORY}/"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/source/lib/${prefix_lib}gdi${suffix_lib_dynamic}" "${OUTPUT_DIRECTORY}/")
endif()

# 第三方库

# lua
find_library(LIB_LUA lua REQUIRED)
list(APPEND LIBRARIES_LINK ${LIB_LUA})

# fmt
if(UNIX) # linux / macos
    find_library(LIB_FMT fmt REQUIRED)
    list(APPEND LIBRARIES_LINK ${LIB_FMT})
endif()

# cpp-httplib
if(UNIX AND NOT APPLE) # linux
    find_library(LIB_CPP_HTTPLIB cpp-httplib REQUIRED)
    list(APPEND LIBRARIES_LINK ${LIB_CPP_HTTPLIB})
endif()

# 将第三方库链接至 main
message(STATUS "LIBRARIES_LINK: ${LIBRARIES_LINK}")
target_link_libraries(jx3calc PRIVATE ${LIBRARIES_LINK})
# 将第三方库复制到输出目录
foreach(lib IN LISTS LIBRARIES_LINK)
    string(REPLACE ${suffix_lib_link} ${suffix_lib_dynamic} lib_dynamic ${lib})
    list(APPEND LIBRARIES_DYNAMIC ${lib_dynamic})
endforeach()
add_custom_command(
    TARGET jx3calc
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${LIBRARIES_DYNAMIC} "${OUTPUT_DIRECTORY}/")

# ########## 处理资源 ##########
set(RESOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/resources/") # 资源文件
file(COPY ${RESOURCE_DIRECTORY} DESTINATION ${OUTPUT_DIRECTORY}) # 复制资源文件到输出目录

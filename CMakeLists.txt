cmake_minimum_required(VERSION 3.10)
project(jx3calc)

# 编译选项

set(CMAKE_CXX_STANDARD 20) # 设置 C++ 标准为 C++20
set(CMAKE_CXX_EXTENSIONS OFF) # 设置 C++ 编译器不使用扩展特性
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 导出编译命令, 用于 clangd

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in debug mode.")
    add_definitions(-DDEBUG)
    add_definitions(-DD_CONSTEXPR_LOG)
    add_definitions(-DD_CONSTEXPR_CHANNELINTERVAL)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # remove default warning level from CMAKE_CXX_FLAGS
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(
        "/utf-8" # 指定源文件编码为 UTF-8
        "/EHsc" # 启用 C++ 异常处理
        "/W4" # 设置警告等级为 4
    )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if(APPLE)
        set(CMAKE_MACOSX_RPATH ON)
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        set(CMAKE_INSTALL_RPATH "@executable_path/")
    else()
        set(CMAKE_SKIP_BUILD_RPATH FALSE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN")
    endif()
else()
    message(FATAL_ERROR "Unsupported compiler type: ${CMAKE_CXX_COMPILER_ID}")
endif()

set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE}") # 输出目录

function(set_output_directory TYPE)
    set(CMAKE_${TYPE}_OUTPUT_DIRECTORY
        ${OUTPUT_DIRECTORY}
        PARENT_SCOPE)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_${TYPE}_OUTPUT_DIRECTORY_${OUTPUTCONFIG}
            ${OUTPUT_DIRECTORY}
            PARENT_SCOPE)
    endforeach()
endfunction()

set_output_directory(RUNTIME) # .exe .dll
set_output_directory(LIBRARY) # .dylib .so
set_output_directory(ARCHIVE) # .lib .a

# 处理 lua
set(LUA_LIB_DIR "${CMAKE_BINARY_DIR}/lua")

function(download_lua URL FILE)
    if(NOT EXISTS ${LUA_LIB_DIR})
        file(MAKE_DIRECTORY ${LUA_LIB_DIR})
        set(LUA_LIB_ZIP "${CMAKE_BINARY_DIR}/${FILE}")
        message(STATUS "Downloading lua ...")
        file(DOWNLOAD ${URL} ${LUA_LIB_ZIP} SHOW_PROGRESS)
        execute_process(COMMAND tar -xf ${LUA_LIB_ZIP} -C ${LUA_LIB_DIR})
    endif()
endfunction()

if(WIN32)
    download_lua("https://nchc.dl.sourceforge.net/project/luabinaries/5.4.2/Windows%20Libraries/Static/lua-5.4.2_Win64_vc17_lib.zip" "lua-5.4.2_Win64_vc17_lib.zip")
    find_library(
        LUA
        NAMES lua54
        PATHS "${LUA_LIB_DIR}")
    include_directories("${LUA_LIB_DIR}/include")
else()
    download_lua("https://www.lua.org/ftp/lua-5.4.6.tar.gz" "lua-5.4.6.tar.gz")
    execute_process(COMMAND bash -c "cd ${LUA_LIB_DIR}/lua-5.4.6 && make && make install INSTALL_TOP=../..")
    find_library(
        LUA
        NAMES lua
        PATHS "${LUA_LIB_DIR}/lib")
    include_directories("${LUA_LIB_DIR}/include")
endif()

# jx3calc 和 gdi 通用头文件
include_directories("${CMAKE_SOURCE_DIR}/source/lib")
# 第三方头文件
include_directories("${CMAKE_SOURCE_DIR}/extern/asio/asio/include")
include_directories("${CMAKE_SOURCE_DIR}/extern/cpp-httplib")
include_directories("${CMAKE_SOURCE_DIR}/extern/crow/include")
include_directories("${CMAKE_SOURCE_DIR}/extern/nlohmann-json/include")
include_directories("${CMAKE_SOURCE_DIR}/extern/sol2/include")

# 构建 jx3calc
file(GLOB_RECURSE MAIN_SOURCES "${CMAKE_SOURCE_DIR}/source/src/*.cpp")
add_executable(jx3calc ${MAIN_SOURCES}) # 编译为可执行文件
target_include_directories(jx3calc PRIVATE "${CMAKE_SOURCE_DIR}/source/include")
target_link_libraries(jx3calc PRIVATE ${LUA}) # 链接 lua 库

# 构建 gdi
if(EXISTS "${CMAKE_SOURCE_DIR}/JX3CalcBEgdi")
    add_subdirectory(JX3CalcBEgdi) # 若子目录存在, 则构建 gdi
    target_link_libraries(jx3calc PRIVATE gdi) # 将 gdi 链接至 main
else()
    find_library(
        GDI_LIB
        NAMES gdi
        PATHS "${CMAKE_SOURCE_DIR}/source/lib")
    if(WIN32)
        set(GDI_DLL gdi.dll)
    elseif(APPLE)
        set(GDI_DLL libgdi.dylib)
    else()
        set(GDI_DLL libgdi.so)
    endif()
    target_link_libraries(jx3calc PRIVATE ${GDI_LIB})
    file(COPY "${CMAKE_SOURCE_DIR}/source/lib/${GDI_DLL}" DESTINATION "${OUTPUT_DIRECTORY}")
endif()

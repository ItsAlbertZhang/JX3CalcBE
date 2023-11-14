cmake_minimum_required(VERSION 3.10)
project(jx3calc)

# ########## Env ##########

if(NOT DEFINED JX3CALC_ENV_OK)
    # compiler
    set(CMAKE_CXX_STANDARD 20) # 设置 C++ 标准为 C++20
    set(CMAKE_CXX_EXTENSIONS OFF) # 设置 C++ 编译器不使用扩展特性
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 导出编译命令, 用于 clangd

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Building in debug mode.")
        add_definitions(-DDEBUG)
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
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")
        if(APPLE)
            set(CMAKE_MACOSX_RPATH ON)
            set(CMAKE_SKIP_BUILD_RPATH FALSE)
            set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
            set(CMAKE_INSTALL_RPATH "@executable_path/")
        elseif(UNIX AND NOT APPLE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++") # 使用 libc++ 标准库
            set(CMAKE_SKIP_BUILD_RPATH FALSE)
            set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
            set(CMAKE_INSTALL_RPATH "$ORIGIN")
        endif()
    else()
        message(FATAL_ERROR "Unsupported compiler type: ${CMAKE_CXX_COMPILER_ID}")
    endif()

    # output directory
    set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE}") # 输出目录

    function(set_output_directory TYPE)
        # Clang
        set(CMAKE_${TYPE}_OUTPUT_DIRECTORY
            ${OUTPUT_DIRECTORY}
            PARENT_SCOPE)
        # MSVC
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

    # Lua
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
        download_lua("https://sourceforge.net/projects/luabinaries/files/5.4.2/Windows%20Libraries/Static/lua-5.4.2_Win64_vc17_lib.zip/download" "lua-5.4.2_Win64_vc17_lib.zip")
        find_library(
            LUA
            NAMES lua54
            PATHS "${LUA_LIB_DIR}")
        include_directories("${LUA_LIB_DIR}/include")
    else()
        download_lua("https://www.lua.org/ftp/lua-5.4.6.tar.gz" "lua-5.4.6.tar.gz")
        execute_process(COMMAND bash -c "cd ${LUA_LIB_DIR}/lua-5.4.6 && make CC=${CMAKE_C_COMPILER} MYCFLAGS='-std=gnu99 -fPIC' && make install INSTALL_TOP=../..")
        find_library(
            LUA
            NAMES lua
            PATHS "${LUA_LIB_DIR}/lib")
        include_directories("${LUA_LIB_DIR}/include")
    endif()

    # include
    include_directories("${CMAKE_SOURCE_DIR}/extern/sol2/include")

    set(JX3CALC_ENV_OK)
endif()

# ########## platform ##########

if(WIN32)
    set(GDI_DLL gdi.dll)
    set(JX3CALC_EXE jx3calc_windows)
elseif(APPLE)
    set(GDI_DLL libgdi.dylib)
    set(JX3CALC_EXE jx3calc_macos)
else()
    set(GDI_DLL libgdi.so)
    set(JX3CALC_EXE jx3calc_linux)
endif()

# ########## private ##########
if(EXISTS "${CMAKE_SOURCE_DIR}/private")
    add_subdirectory(private) # 若 private 目录存在, 则构建
    set(GDI gdi)
else()
    find_library(
        GDI
        NAMES gdi
        PATHS "${CMAKE_SOURCE_DIR}/source/lib")
    file(COPY "${CMAKE_SOURCE_DIR}/source/lib/${GDI_DLL}" DESTINATION "${OUTPUT_DIRECTORY}")
endif()

# ########## jx3calc ##########
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DD_CONSTEXPR_LOG)
    add_definitions(-DD_CONSTEXPR_CHANNELINTERVAL)
endif()

file(GLOB_RECURSE MAIN_SOURCES "${CMAKE_SOURCE_DIR}/source/src/*.cpp")
add_executable(${JX3CALC_EXE} ${MAIN_SOURCES}) # 编译为可执行文件

target_include_directories(${JX3CALC_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/extern/asio/asio/include")
target_include_directories(${JX3CALC_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/extern/cpp-httplib")
target_include_directories(${JX3CALC_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/extern/crow/include")
target_include_directories(${JX3CALC_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/extern/nlohmann-json/include")
target_include_directories(${JX3CALC_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/source/lib")
target_include_directories(${JX3CALC_EXE} PRIVATE "${CMAKE_SOURCE_DIR}/source/include")

target_link_libraries(${JX3CALC_EXE} PRIVATE ${GDI}) # 链接 gdi 库
target_link_libraries(${JX3CALC_EXE} PRIVATE ${LUA}) # 链接 lua 库
if(DEFINED CROW_ENABLE_SSL)
    add_definitions(-DCROW_ENABLE_SSL)
    target_link_libraries(${JX3CALC_EXE} PRIVATE ssl crypto) # 链接 ssl 库
endif()

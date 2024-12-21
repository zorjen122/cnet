# 设置最低版本的 CMake
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(cns VERSION 1.0 LANGUAGES CXX)

# 设置 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++17 -Wall -g")
# 指定源文件目录（假设所有的 .cc 文件都位于 src 目录下）
file(GLOB SOURCES "src/*.cc" "src/http/*.cc")

# spdlog 暂时配置
include_directories(/usr/local/spdlog/include)
link_directories(/usr/local/spdlog/lib)


# 查找 pthread 和 dl 库
find_package(Threads REQUIRED)

# 创建共享库 cnet
add_library(cnet SHARED ${SOURCES})

# 创建静态库 cnetStatic
add_library(cnetStatic STATIC ${SOURCES})

# 将 pthread 和 dl 库链接到共享库
target_link_libraries(cnet PRIVATE Threads::Threads dl spdlog)
target_link_libraries(cnetStatic PRIVATE Threads::Threads dl spdlog)


# 设置共享库的输出目录
set_target_properties(cnet PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

# 设置静态库的输出目录
set_target_properties(cnetStatic PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/http/)



# 查找测试文件（test/test*.cc）
file(GLOB TEST_SOURCES "test/test*.cc")

# 为每个测试文件创建一个独立的可执行文件
foreach(test_file ${TEST_SOURCES})
    # 获取测试文件的名称（不带路径和扩展名）
    get_filename_component(test_name ${test_file} NAME_WE)

    # 创建可执行文件
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name})

    # 链接共享库和静态库（根据需要选择一个）
    target_link_libraries(${test_name} PRIVATE cnet cnetStatic Threads::Threads dl spdlog)

    # 设置测试程序的输出目录
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )
endforeach()

# 可选：安装目标（可根据需要取消注释）
# install(TARGETS cnet DESTINATION lib)
# install(TARGETS cnetStatic DESTINATION lib)
# install(TARGETS ${TEST_SOURCES} DESTINATION bin)

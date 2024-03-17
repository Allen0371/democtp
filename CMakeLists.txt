cmake_minimum_required(VERSION 3.0)
# CMake 最低版本号要求

# PROJECT_SOURCE_DIR：工程的根目录
 # PROJECT_BINARY_DIR：运行cmake命令的目录，通常为${PROJECT_SOURCE_DIR}/build
# PROJECT_NAME：返回通过 project 命令定义的项目名称
# CMAKE_CURRENT_SOURCE_DIR：当前处理的 CMakeLists.txt 所在的路径
# CMAKE_CURRENT_BINARY_DIR：target 编译目录
 # CMAKE_CURRENT_LIST_DIR：CMakeLists.txt 的完整路径
 # EXECUTABLE_OUTPUT_PATH：重新定义目标二进制可执行文件的存放位置
 # LIBRARY_OUTPUT_PATH：重新定义目标链接库文件的存放位置
# 项目信息
project (ProjectCTP)
set(CMAKE_BUILD_TYPE Debug)
#set(SRC_LIST)
# //set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
add_compile_options(-fPIC -std=c++17 -Wall -Werror -lpthread)
include_directories(./include)
include_directories(./ctp)

aux_source_directory(. SRC_LIST)
aux_source_directory(./src SRC_LIST)

link_directories(./ctp)

# link_libraries(./ctp/thostmduserapi_se)
# 指定生成目标
add_executable(catCTP ${SRC_LIST})
target_link_libraries(catCTP thostmduserapi_se thosttraderapi_se LinuxDataCollect pthread iconv)
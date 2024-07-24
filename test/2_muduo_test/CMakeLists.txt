



# 配置头文件搜索路径
include_directories()

# 配置库文件搜索路径
link_directories()

# 配置生成的可执行文件
# 设置需要编译的源文件列表
set(SRC_LIST ./EchoServer.cc)

# 把 指定路径. 下的所有源文件名字放入变量名SRC_LIST里面
# aux_source_directory(. SRC_LIST)

# 设置可执行文件最终存储的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 生成可执行文件echoserver
# 表示生成可执行文件server，由SRC_LIST变量所定义的源文件编译而来
add_executable(echoserver ${SRC_LIST})

# 链接库文件
# 表示server这个目标程序，需要连接muduo_net muduo_base pthread这三个库文件
target_link_libraries(echoserver muduo_net muduo_base pthread)
# Cmakelists示例
# 项目名称  
PROJECT(glue)
# 指定cmake最低版本
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
# 指定编译器
SET(CMAKE_C_COMPILER "gcc")
SET(CMAKE_CXX_COMPILER "g++")
# 指定c++标准
SET(CMAKE_CXX_FLAGS "-std=c++17")

include_directories(
    include
    ThirdParty/json/include
)

# 指定源文件    
add_executable(glue src/testGlueCode.cpp)
# 指定头文件路径示例

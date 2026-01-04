cmake_minimum_required(VERSION 3.15)
project(TimerDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 可执行文件
add_executable(TimerDemo
    main.cpp
    Timer.hpp
    TimerManager.hpp
)

# 编译器选项
target_compile_options(TimerDemo PRIVATE
    -Wall
    -Wextra
    -Werror
    -O2
)

# 线程库
find_package(Threads REQUIRED)
target_link_libraries(TimerDemo PRIVATE Threads::Threads)
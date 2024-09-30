cmake_minimum_required(VERSION 3.20)

project(delivery_mechanisms VERSION 1 LANGUAGES CXX)

# set build_type debug
add_definitions("-Wall -g")
set(CMAKE_BUILD_TYPE Debug)

# Find requirements
if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED PATHS /home/think/Fast-DDS/install/lib/cmake)
endif()

if(NOT foonathan_memory_FOUND)
    find_package(foonathan_memory REQUIRED PATHS /home/think/Fast-DDS/install/lib/foonathan_memory/cmake)
endif()

if(NOT fastdds_FOUND)
    find_package(fastdds 3 REQUIRED PATHS /home/think/Fast-DDS/install/share/fastdds/cmake)
endif()

# Check c++11
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(NOT SUPPORTS_CXX11)
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

# add include path
include_directories(
    ${PROJECT_SOURCE_DIR}

)

message(STATUS "Configuring delivery mechanisms example...")
file(GLOB DELIVERY_MECHANISMS_SOURCES_CPP "src/*.cpp")
file(GLOB REQUEST_REPLY_TYPES_SOURCES_CPP "types/*.cpp")
file(GLOB REQUEST_REPLY_TYPES_SOURCES_Cxx "types/*.cxx")
file(GLOB DELIVERY_MECHANISMS_HEADERS_HPP "include/*.hpp")

add_executable(delivery_mechanisms
    ${DELIVERY_MECHANISMS_SOURCES_CPP}
    ${REQUEST_REPLY_TYPES_SOURCES_CPP}
    ${REQUEST_REPLY_TYPES_SOURCES_Cxx}
    ${DELIVERY_MECHANISMS_HEADERS_HPP})

target_link_libraries(delivery_mechanisms fastdds fastcdr)

install(TARGETS delivery_mechanisms
    RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/${BIN_INSTALL_DIR})

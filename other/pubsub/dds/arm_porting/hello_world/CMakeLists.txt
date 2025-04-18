# Copyright 2024 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.20)

project(fastdds_hello_world_example VERSION 1 LANGUAGES CXX)

set(CMAKE_C_COMPILER /usr/local/arm/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/local/arm/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++)

set(OPENSSL_ROOT_DIR /home/think/Fast-DDS/arm_install/lib)
# set(OpenSSL_INCLUDE_DIR /home/think/Fast-DDS/arm_install/include/openssl)
# set(OpenSSL_SSL_LIBRARY  /home/think/Fast-DDS/arm_install/lib/arm_install/lib/libssl.so)
# set(OpenSSL_CRYPTO_LIBRARY /home/think/Fast-DDS/arm_install/lib/arm_install/lib/libcrypto.so)

# find_package(OpenSSL REQUIRED PATHS /home/think/Fast-DDS/arm_install/lib/)
# message()
# set(OPENSSL_ROOT_DIR /home/think/Fast-DDS/arm_install/lib)
# find_package(OpenSSL REQUIRED)

# include_directories(${OPENSSL_INLCUDE_DIR})

# Find requirements
# if(NOT fastcdr_FOUND)
#     find_package(fastcdr 2 REQUIRED)
# endif()

# if(NOT fastdds_FOUND)
#     find_package(fastdds 3 REQUIRED)
# endif()

if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED PATHS /home/think/Fast-DDS/arm_install/lib/cmake)
endif()

if(NOT foonathan_memory_FOUND)
    find_package(foonathan_memory REQUIRED PATHS /home/think/Fast-DDS/arm_install/lib/foonathan_memory/cmake)
endif()

if(NOT tinyxml2_FOUND)
    find_package(tinyxml2 REQUIRED PATHS /home/think/Fast-DDS/arm_install/lib/cmake/)
endif()

if(NOT fastdds_FOUND)
    find_package(fastdds 3 REQUIRED PATHS /home/think/Fast-DDS/arm_install/share/fastdds/cmake)
endif()


#Check C++11
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(NOT SUPPORTS_CXX11)
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

message(STATUS "Configuring hello world example...")
file(GLOB HELLO_WORLD_SOURCES_CXX "*.cxx")
file(GLOB HELLO_WORLD_SOURCES_CPP "*.cpp")

add_executable(hello_world ${HELLO_WORLD_SOURCES_CXX} ${HELLO_WORLD_SOURCES_CPP})
target_compile_definitions(hello_world PRIVATE
    $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">>:__DEBUG>
    $<$<BOOL:${INTERNAL_DEBUG}>:__INTERNALDEBUG> # Internal debug activated.
    $<$<BOOL:${SHM_TRANSPORT_DEFAULT}>:SHM_TRANSPORT_BUILTIN> # Enable SHM as built-in transport
)
target_link_libraries(hello_world fastdds fastcdr)
install(TARGETS hello_world
    RUNTIME DESTINATION ${DATA_INSTALL_DIR}/fastdds/examples/cpp/hello_world/${BIN_INSTALL_DIR})

# Copy the XML files over to the build directory
file(GLOB_RECURSE XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.xml)
# for each xml file detected
foreach(XML_FILE_COMPLETE_PATH ${XML_FILES})
    # obtain the file name
    get_filename_component(XML_FILE ${XML_FILE_COMPLETE_PATH} NAME_WE)
    # copy the file from src to build folders
    configure_file(
        ${XML_FILE_COMPLETE_PATH}                   # from full src path
        ${CMAKE_CURRENT_BINARY_DIR}/${XML_FILE}.xml # to relative build path
        COPYONLY)
    install(FILES ${XML_FILE_COMPLETE_PATH}
        DESTINATION ${DATA_INSTALL_DIR}/fastdds/examples/cpp/hello_world/${BIN_INSTALL_DIR})
endforeach()

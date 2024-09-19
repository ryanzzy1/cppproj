cmake_minimum_required(VERSION 3.20)

project(fastdds_request_reply VERSION 1 LANGUAGES CXX)

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


message(STATUS "Configuring ${PROJECT_NAME}...")
file(GLOB REQUEST_REPLY_TYPES_SOURCES_CXX "types/*.cxx")
file(GLOB REQUEST_REPLY_SOURCES_CXX "src/*.cxx")
file(GLOB REQUEST_REPLY_TYPES_SOURCES_CPP "types/*.cpp")
file(GLOB REQUEST_REPLY_SOURCES_CPP "src/*.cpp" )
file(GLOB REQUEST_REPLY_HEADERS_HPP "include/*.hpp")

add_executable(request_reply ${REQUEST_REPLY_SOURCES_CXX} ${REQUEST_REPLY_SOURCES_CPP} ${REQUEST_REPLY_TYPES_SOURCES_CXX} ${REQUEST_REPLY_TYPES_SOURCES_CPP} ${REQUEST_REPLY_HEADERS_HPP})
target_compile_definitions(request_reply PRIVATE
    $<$<AND:$<NOT:$<BOOL:${WIN32}>>,$<STREQUAL:"${CMAKE_BUILD_TYPE}", "Debug">>:__DEBUG>
    $<$<BOOL:${INTERNAL_DEBUG}>:__INTERNALDEBUG> # Internal debug activated
    $<$<BOOL:${SHM_TRANSPORT_DEFAULT}>:SHM_TRANSPORT_BUILTIN>  # Enable SHM as built-in transport
)

target_link_libraries(request_reply fastdds fastcdr)
install(TARGETS request_reply
    RUNTIME DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BIN_INSTALL_DIR})

# copy xml files to build dir
file(GLOB_RECURSE XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.xml)
# for each xml file detected
foreach(XML_FILE_COMPLETE_PATH ${XML_FILES})
    # obtain the file name 
    get_filename_component(XML_FILE ${XML_FILE_COMPLETE_PATH} NAME_WE)
    # copy the file from src to build folders
    configure_file(
        ${XML_FILE_COMPLETE_PATH}
        ${CMAKE_CURRENT_BINARY_DIR}/${XML_FILE}.xml
        COPYONLY)
        install(FILES ${XMLE_FILE_COMPLETE_PATH}
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${BIN_INSTALL_DIR})
endforeach()

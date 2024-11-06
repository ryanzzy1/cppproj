
cmake_minimum_required (VERSION 2.8.7)
project (vSomeIPReqRes)

find_package(Threads REQUIRED)

set(VSOMEIP_NAME "vsomeip3")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(${VSOMEIP_INCLUDE_DIRS})

set(EXAMPLE_CONFIG_FILES
    "../../config/vsomeip.json"
    "../../config/vsomeip-local.json"
    "../../config/vsomeip-tcp-client.json"
    "../../config/vsomeip-tcp-service.json"
    "../../config/vsomeip-udp-client.json"
    "../../config/vsomeip-udp-service.json"
)

# Examples
add_executable(request-sample request-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(request-sample ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY} ${VSOMEIP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(response-sample response-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(response-sample ${VSOMEIP_NAME} ${Boost_LIBRARIES} ${DL_LIBRARY} ${VSOMEIP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}) 
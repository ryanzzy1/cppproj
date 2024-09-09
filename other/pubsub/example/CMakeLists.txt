cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_FLAGS "")

set(EXAMPLE_CONFIG_FILES
    "./config/vsomeip.json"
    "./config/vsomeip-local.json"
    "./config/vsomeip-tcp-client.json"
    "./config/vsomeip-tcp-service.json"
    "./config/vsomeip-udp-client.json"
    "./config/vsomeip-udp-service.json"
)

find_package(vsomeip3 3.4.10 REQUIRED)
find_package(Boost 1.55 COMPONENTS system thread log REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${VSOMEIP_INCLUDE_DIRS}
)

add_executable(subscribe-sample ./src/subscribe-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(subscribe-sample vsomeip3 ${Boost_LIBRARIES} ${DL_LIBRARY})

add_executable(notify-sample ./src/notify-sample.cpp ${EXAMPLE_CONFIG_FILES})
target_link_libraries(notify-sample vsomeip3 ${Boost_LIBRARIES} ${DL_LIBRARY})


cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_FLAGS "")

set(EXAMPLE_CONFIG_FILES
    "./config/vsomeip-local.json"
)

find_package(vsomeip3 3.4.10 REQUIRED)
find_package(Boost 1.55 COMPONENTS system thread log REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${VSOMEIP_INCLUDE_DIRS}
)

add_executable(subscribe-sample 
    ./src/client_subscribe.cpp 
    ./src/client_sample.cpp 
    ./include/client_sample.hpp
    ./include/sample-ids.hpp
    ${EXAMPLE_CONFIG_FILES})
target_link_libraries(subscribe-sample vsomeip3 ${Boost_LIBRARIES} ${DL_LIBRARY})


add_executable(notify-sample 
    ./src/service_notify.cpp 
    ./src/service_sample.cpp
    ./include/service_sample.hpp
    ./include/sample-ids.hpp 
    ${EXAMPLE_CONFIG_FILES})
target_link_libraries(notify-sample vsomeip3 ${Boost_LIBRARIES} ${DL_LIBRARY})



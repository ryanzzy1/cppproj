cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_FLAGS "")

find_package(vsomeip3 3.4.10 REQUIRED)
find_package(Boost 1.55 COMPONENTS system thread log REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${VSOMEIP_INCLUDE_DIRS}
)

add_executable(service ../src/service.cpp)
target_link_libraries(service vsomeip3 ${Boost_LIBRARIES})

add_executable(client ../src/client-app.cpp)
target_link_libraries(client vsomeip3 ${Boost_LIBRARIES})

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11")

set(SOURCES main.cpp src/pushbullet.cpp src/student.cpp)

add_executable(main ${SOURCES})

add_subdirectory(cpr)
include_directories(${CPR_INCLUDE_DIRS} include)
target_link_libraries(main ${CPR_LIBRARIES} curl myhtml pthread)

cmake_minimum_required(VERSION 3.10)
project(Server2)

add_executable(server2 server2.cpp system_info.cpp)
target_link_libraries(server2 X11 Xinerama)
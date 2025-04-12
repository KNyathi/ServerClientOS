cmake_minimum_required(VERSION 3.10)
project(Server1)

add_executable(server1 server1.cpp system_info.cpp)
target_link_libraries(server1 X11 Xinerama Xrandr)